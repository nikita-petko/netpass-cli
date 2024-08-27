#include <np/api.h>
#include <np/config.h>
#include <np/http.h>
#include <np/scene.h>
#include <np/util.h>

#include <np/log.h>

#include "api/api_Urls.h"

namespace {
nn::os::Thread	   s_DownloadThread;
nn::os::LightEvent s_DownloadEvent;	 // For manual triggering of the download thread
bool			   s_DownloadThreadRunning = false;

nn::os::LightEvent s_WaitForDownloadEvent;	// For waiting for a download to complete

#ifdef NN_BUILD_DEBUG
nn::fnd::TimeSpan s_DownloadInterval = nn::fnd::TimeSpan::FromSeconds(30);
#else
nn::fnd::TimeSpan s_DownloadInterval = nn::fnd::TimeSpan::FromMinutes(5);
#endif

nn::Result
WriteMessageToBox(u8* messageBuffer)
{
	nn::Result result;
	size_t	   readLength = 0;

	NN_LOG_INFO("Writing message to box, message buffer: %p", messageBuffer);

	// Sanity check for message validity
	if (!np::util::IsValidCecMessage(messageBuffer))
	{
		NN_LOG_ERROR("Invalid message");

		return np::api::ResultInvalidCecMessage();
	}

	nn::cec::CecMessageHeader* messageHeader = reinterpret_cast<nn::cec::CecMessageHeader*>(messageBuffer);

	// If the message header has no receive date, set it to the current date
	if (!messageHeader->recvDate.year)
		messageHeader->recvDate = nn::fnd::DateTime::GetNow().GetParameters();

	// First allocation for just the header, in order to forward allocate the full size of the message box
	NN_LOG_INFO("Determining message box size for title %08X", messageHeader->cecTitleId);

	u8* boxBuffer = reinterpret_cast<u8*>(std::malloc(nn::cec::CEC_SIZEOF_BOXINFO_HEADER));
	if (boxBuffer == NULL)
	{
		NN_LOG_ERROR("Failed to allocate memory for box buffer");

		return np::api::ResultMemoryAllocationFailed();
	}

	NN_LOG_INFO("Opening and reading inbox info for title %08X", messageHeader->cecTitleId);

	result = nn::cec::detail::OpenAndReadFile(boxBuffer,
											  nn::cec::CEC_SIZEOF_BOXINFO_HEADER,
											  &readLength,
											  messageHeader->cecTitleId,
											  nn::cec::FILETYPE_INBOX_INFO,
											  nn::cec::FILEOPT_READ | nn::cec::FILEOPT_NOCHECK);
	if (result.IsFailure())
	{
		NN_LOG_ERROR("Failed to open and read inbox info:");
		NN_DBG_PRINT_RESULT(result);

		std::free(boxBuffer);

		return result;
	}

	// Calculate the full size of the message box
	u32 maxMessages = reinterpret_cast<nn::cec::CecBoxInfoHeader*>(boxBuffer)->messNumMax;
	u32 maxBoxSize	= nn::cec::CEC_SIZEOF_BOXINFO_HEADER + (maxMessages * sizeof(nn::cec::CecMessageHeader));

	NN_LOG_INFO("Allocating memory for full message box, max messages: %d, max size: %d", maxMessages, maxBoxSize);

	std::free(boxBuffer);

	// Allocate the full sized message box
	boxBuffer = reinterpret_cast<u8*>(std::malloc(maxBoxSize));
	if (boxBuffer == NULL)
	{
		NN_LOG_ERROR("Failed to allocate memory for box buffer");

		return np::api::ResultMemoryAllocationFailed();
	}

	// Open the inbox info file again to check if the message already exists
	NN_LOG_INFO("Opening and reading inbox info for title %08X", messageHeader->cecTitleId);

	result = nn::cec::detail::OpenAndReadFile(boxBuffer,
											  maxBoxSize,
											  &readLength,
											  messageHeader->cecTitleId,
											  nn::cec::FILETYPE_INBOX_INFO,
											  nn::cec::FILEOPT_READ | nn::cec::FILEOPT_NOCHECK);
	if (result.IsFailure())
	{
		NN_LOG_ERROR("Failed to open and read inbox info:");
		NN_DBG_PRINT_RESULT(result);

		std::free(boxBuffer);

		return result;
	}

	nn::cec::CecBoxInfoHeader* boxInfoHeader = reinterpret_cast<nn::cec::CecBoxInfoHeader*>(boxBuffer);
	nn::cec::CecMessageHeader* boxMessages	 = reinterpret_cast<nn::cec::CecMessageHeader*>(boxBuffer + sizeof(nn::cec::CecBoxInfoHeader));

	NN_LOG_INFO("Checking if message already exists in inbox for title %08X", messageHeader->cecTitleId);

	// Check if the message already exists in the inbox
	for (int i = 0; i < boxInfoHeader->messNum; i++)
	{
		if (0 == std::memcmp(boxMessages[i].messageId, messageHeader->messageId, nn::cec::CEC_SIZEOF_MESSAGEID))
		{
			NN_LOG_INFO("Message already exists in inbox");

			std::free(boxBuffer);

			return nn::ResultSuccess();
		}
	}

	NN_LOG_INFO("Checking if inbox is full for title %08X, current messages: %d, max messages: %d",
				messageHeader->cecTitleId,
				boxInfoHeader->messNum,
				boxInfoHeader->messNumMax);

	// Check if the inbox is full (sanity check, ::DoDownloadBoxes already checks this, but just in case a message is popped before this
	// check)
	if (boxInfoHeader->messNum >= boxInfoHeader->messNumMax)
	{
		NN_LOG_ERROR("Inbox is full");

		std::free(boxBuffer);

		return np::api::ResultMessageBoxAtMaxCapacity();
	}

	NN_LOG_INFO("Checking if message is too large for inbox for title %08X, message size: %d, max size: %d",
				messageHeader->cecTitleId,
				messageHeader->messSize,
				boxInfoHeader->messSizeMax);

	// Check if the message is too large for the inbox
	if (boxInfoHeader->messSizeMax < messageHeader->messSize)
	{
		NN_LOG_ERROR("Message too large for inbox");

		std::free(boxBuffer);

		return np::api::ResultMessageTooLargeForBox();
	}

	NN_LOG_INFO("Checking if message would exceed inbox size for title %08X, message size: %d, current size: %d, max size: %d",
				messageHeader->cecTitleId,
				messageHeader->messSize,
				boxInfoHeader->boxSize,
				boxInfoHeader->boxSizeMax);

	// Check if the message would cause the inbox to exceed its maximum size
	if (boxInfoHeader->boxSize + messageHeader->messSize > boxInfoHeader->boxSizeMax)
	{
		NN_LOG_ERROR("Message would exceed inbox size");

		std::free(boxBuffer);

		return np::api::ResultMessageTooLargeForBox();
	}

	std::free(boxBuffer);

	NN_LOG_INFO("Writing message to inbox for title %08X", messageHeader->cecTitleId);

	// Write the message to the inbox
	{
		NN_LOG_INFO("Getting HMAC key for title %08X", messageHeader->cecTitleId);

		// Open the message box info file to fetch the HMAC key
		nn::cec::MessageBoxInfo messageBoxInfo;
		result = np::util::GetCecMessageBoxInfo(messageHeader->cecTitleId, &messageBoxInfo);
		if (result.IsFailure())
		{
			NN_LOG_ERROR("Failed to get message box info:");
			NN_DBG_PRINT_RESULT(result);

			return result;
		}

		NN_LOG_INFO("Updating message flags for title %08X and writing message", messageHeader->cecTitleId);

		// Update the message flags to indicate that it is unread and new
		messageHeader->flagUnread = true;
		messageHeader->flagNew	  = true;

		result = nn::cec::detail::WriteMessageWithHmac(messageHeader->cecTitleId,
															nn::cec::CEC_BOXTYPE_INBOX,
															messageHeader->messageId,
															nn::cec::CEC_SIZEOF_MESSAGEID,
															messageBuffer,
															messageHeader->messSize,
															reinterpret_cast<u8*>(messageBoxInfo.hmacKey));
		if (!result.IsSuccess())
		{
			NN_LOG_ERROR("Failed to write message");
			NN_DBG_PRINT_RESULT(result);

			return result;
		}

		NN_LOG_INFO("Checking if message was written for title %08X", messageHeader->cecTitleId);

		u8 temp = 0;

		// Determine if the message was written (sometimes WriteMessage will not error even if it actually errored)
		result = nn::cec::detail::ReadMessage(messageHeader->cecTitleId,
												   nn::cec::CEC_BOXTYPE_INBOX,
												   messageHeader->messageId,
												   nn::cec::CEC_SIZEOF_MESSAGEID,
												   &readLength,
												   &temp,
												   1);
		if (result.IsFailure())
		{
			NN_LOG_ERROR("Failed to read message:");
			NN_DBG_PRINT_RESULT(result);

			return result;
		}
	}

	NN_LOG_INFO("Re-opening inbox info for title %08X to check if metadata needs to be updated", messageHeader->cecTitleId);

	// Re-open the message box info file to determine if metadata needs to be updated
	boxBuffer = reinterpret_cast<u8*>(std::malloc(maxBoxSize));
	if (boxBuffer == NULL)
	{
		NN_LOG_ERROR("Failed to allocate memory for box buffer");

		return np::api::ResultMemoryAllocationFailed();
	}

	NN_LOG_INFO("Opening and reading inbox info for title %08X", messageHeader->cecTitleId);

	result = nn::cec::detail::OpenAndReadFile(boxBuffer,
												   maxBoxSize,
												   &readLength,
												   messageHeader->cecTitleId,
												   nn::cec::FILETYPE_INBOX_INFO,
												   nn::cec::FILEOPT_READ | nn::cec::FILEOPT_NOCHECK);
	if (result.IsFailure())
	{
		NN_LOG_ERROR("Failed to open and read inbox info:");
		NN_DBG_PRINT_RESULT(result);

		std::free(boxBuffer);

		return result;
	}

	boxInfoHeader = reinterpret_cast<nn::cec::CecBoxInfoHeader*>(boxBuffer);
	boxMessages	  = reinterpret_cast<nn::cec::CecMessageHeader*>(boxBuffer + sizeof(nn::cec::CecBoxInfoHeader));

	NN_LOG_INFO("Checking if message already exists in inbox for title %08X", messageHeader->cecTitleId);

	bool foundBox = false;
	for (int i = 0; i < boxInfoHeader->messNum; i++)
	{
		if (0 == std::memcmp(boxMessages[i].messageId, messageHeader->messageId, nn::cec::CEC_SIZEOF_MESSAGEID))
		{
			foundBox = true;
			break;
		}
	}

	if (!foundBox)
	{
		NN_LOG_INFO("Writing message metadata to inbox for title %08X", messageHeader->cecTitleId);

		void* address = boxBuffer + sizeof(nn::cec::CecBoxInfoHeader) + (boxInfoHeader->messNum * sizeof(nn::cec::CecMessageHeader));
		std::memcpy(address, messageBuffer, sizeof(nn::cec::CecMessageHeader));

		NN_LOG_INFO(
			"Updating inbox info and writing to file for title %08X, current messages: %d, current box info size: %d, current box size: %d",
			messageHeader->cecTitleId,
			boxInfoHeader->messNum,
			boxInfoHeader->boxInfoSize,
			boxInfoHeader->boxSize);

		boxInfoHeader->messNum++;
		boxInfoHeader->boxInfoSize += sizeof(nn::cec::CecMessageHeader);
		boxInfoHeader->boxSize += messageHeader->messSize;

		result = nn::cec::detail::OpenAndWriteFile(boxBuffer,
														boxInfoHeader->boxInfoSize,
														messageHeader->cecTitleId,
														nn::cec::FILETYPE_INBOX_INFO,
														nn::cec::FILEOPT_WRITE | nn::cec::FILEOPT_NOCHECK);
		if (result.IsFailure())
		{
			NN_LOG_ERROR("Failed to write inbox info:");
			NN_DBG_PRINT_RESULT(result);

			std::free(boxBuffer);

			return result;
		}
	}

	std::free(boxBuffer);

	// Update the green notification status of the title
	{
		NN_LOG_INFO("Updating title notification status for title %08X", messageHeader->cecTitleId);

		// Re-open the message box info in case it was updated
		nn::cec::MessageBoxInfo messageBoxInfo;
		result = np::util::GetCecMessageBoxInfo(messageHeader->cecTitleId, &messageBoxInfo);
		if (result.IsFailure())
		{
			NN_LOG_ERROR("Failed to get message box info:");
			NN_DBG_PRINT_RESULT(result);

			return result;
		}

		NN_LOG_INFO("Updating box information and writing to file for title %08X", messageHeader->cecTitleId);

		messageBoxInfo.lastReceived = nn::fnd::DateTime::GetNow().GetParameters();

		// Set the unread flag (on the original NetPass, this is called flag3)
		messageBoxInfo.flag1 = true;

		result = nn::cec::detail::OpenAndWriteFile(reinterpret_cast<u8*>(&messageBoxInfo),
														sizeof(nn::cec::MessageBoxInfo),
														messageHeader->cecTitleId,
														nn::cec::FILETYPE_MESSAGE_BOX_INFO,
														nn::cec::FILEOPT_WRITE | nn::cec::FILEOPT_NOCHECK);
		if (result.IsFailure())
		{
			NN_LOG_ERROR("Failed to write message box info:");
			NN_DBG_PRINT_RESULT(result);
		}
	}

	return result;
}

void
DoDownloadBoxes()
{
	nn::Result result;
	u32		   totalMessages = 0;

	nn::cec::MessageBoxList messageBoxList;
	result = np::util::GetCecMessageBoxList(&messageBoxList);
	if (!result.IsSuccess())
	{
		NN_LOG_ERROR("Failed to get message box list:");
		NN_DBG_PRINT_RESULT(result);

		PRINTF_DISPLAY1("Error downloading messages");

		s_WaitForDownloadEvent.Signal();
		s_WaitForDownloadEvent.ClearSignal();

		return;
	}

	np::config::ClearIgnoredTitles(&messageBoxList);

	if (messageBoxList.DirNum == 0)
	{
		NN_LOG_INFO("No message boxes to download");

		PRINTF_DISPLAY1("No message boxes to download");
		PRINTF_DISPLAY1("Please enable a title in the settings");

		s_WaitForDownloadEvent.Signal();
		s_WaitForDownloadEvent.ClearSignal();

		return;
	}

	for (int i = 0; i < messageBoxList.DirNum; i++)
	{
		u8*				 titleIdStr = messageBoxList.DirName[i];
		nn::cec::TitleId titleId	= std::strtol(reinterpret_cast<const char*>(titleIdStr), NULL, 16);

		nn::cec::CecBoxInfoHeader boxInfo;
		result = np::util::GetCecMessageBoxHeader(titleId, nn::cec::CEC_BOXTYPE_INBOX, &boxInfo);
		if (result.IsFailure())
		{
			NN_LOG_ERROR("Failed to get message box header:");
			NN_DBG_PRINT_RESULT(result);

			PRINTF_DISPLAY1("Error downloading messages for %s", titleIdStr);

			continue;
		}

		u32 boxMessages = boxInfo.messNum;
		u32 maxMessages = boxInfo.messNumMax;

		PRINTF_DISPLAY1("Checking inbox %s", titleIdStr);

		char8 url[sizeof(API_URL_POP_MESSAGE) + 16];
		nn::nstd::TSNPrintf(url, sizeof(url), API_URL_POP_MESSAGE, titleIdStr);

		if (boxMessages >= maxMessages)
		{
			PRINTF_DISPLAY1("Inbox %s is full, skipping...", titleIdStr);

			s_WaitForDownloadEvent.Signal();
			s_WaitForDownloadEvent.ClearSignal();

			continue;
		}

		s32 statusCode = 200;
		while (statusCode == 200 && boxMessages < maxMessages)
		{
			u8 response[np::util::CEC_MESSAGE_MAX_SIZE];
			result = np::http::Get(url, response, sizeof(response), &statusCode);

			if (result.IsFailure() && result != nn::http::ResultBodyBufShortage())
			{
				NN_LOG_ERROR("Failed to get message:");
				NN_DBG_PRINT_RESULT(result);

				PRINTF_DISPLAY1("Error downloading messages");

				s_WaitForDownloadEvent.Signal();
				s_WaitForDownloadEvent.ClearSignal();

				return;
			}

			if (statusCode == 200)
			{
				PRINTF_DISPLAY1("Got message for %s", titleIdStr);

				result = WriteMessageToBox(response);
				if (result.IsFailure())
				{
					NN_LOG_ERROR("Failed to write message:");
					NN_DBG_PRINT_RESULT(result);

					PRINTF_DISPLAY1("Error downloading messages");

					s_WaitForDownloadEvent.Signal();
					s_WaitForDownloadEvent.ClearSignal();

					return;
				}

				boxMessages++;
				totalMessages++;
			}
		}

		np::scene::GetRequestPassesScene()->UpdateTitleText(titleId, boxMessages, maxMessages);
	}

	s_WaitForDownloadEvent.Signal();
	s_WaitForDownloadEvent.ClearSignal();

	PRINTF_DISPLAY1("Downloaded %d messages", totalMessages);
}

void
downloadThreadLoop(uptr param)
{
	NN_UNUSED_VAR(param);

	while (s_DownloadThreadRunning)
	{
		// Do work...
		DoDownloadBoxes();

		s_DownloadEvent.TryWait(s_DownloadInterval);  // Wait for signal, or set reloop time
		s_DownloadEvent.ClearSignal();
	}
}
}  // namespace

namespace np { namespace api {

	nn::Result StartInboxDownloadThread(void)
	{
		NN_LOG_INFO("Starting background thread for downloading messages");

		PRINTF_DISPLAY1("Downloading messages in the background");
#ifdef NN_BUILD_DEBUG
		PRINTF_DISPLAY1("every 30 seconds...");
#else
		PRINTF_DISPLAY1("every 5 minutes...");
#endif

		s_DownloadEvent.Initialize(true);
		s_WaitForDownloadEvent.Initialize(true);

		s_DownloadThreadRunning = true;

		return s_DownloadThread.TryStartUsingAutoStack(::downloadThreadLoop, NULL, 8 * 1024, 0);
	}

	void FinalizeInboxDownloadThread(void)
	{
		NN_LOG_INFO("Finalizing background thread for downloading messages");

		if (s_DownloadThreadRunning)
		{
			s_DownloadThreadRunning = false;

			s_DownloadEvent.Signal();
			s_DownloadEvent.Finalize();

			s_WaitForDownloadEvent.Signal();
			s_WaitForDownloadEvent.Finalize();

			s_DownloadThread.Join();
			s_DownloadThread.Finalize();
		}
	}

	void TriggerInboxDownload(void)
	{
		NN_LOG_INFO("Triggering inbox download");

		s_DownloadEvent.Signal();
	}

	void WaitForInboxDownloadComplete(void)
	{
		NN_LOG_INFO("Waiting for inbox download to complete");

		s_WaitForDownloadEvent.Wait();
	}

}}	// namespace np::api
