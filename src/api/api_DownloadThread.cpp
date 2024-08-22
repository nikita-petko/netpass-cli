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

bool
IsValidMessage(u8* data)
{
	nn::cec::CecMessageHeader* header = reinterpret_cast<nn::cec::CecMessageHeader*>(data);

	if (header->magic16 != nn::cec::MESSAGE_MAGIC)
	{
		NN_LOG_ERROR("Invalid magic16");

		return false;
	}
	if (header->messSize != header->headerSize + header->bodySize + 0x20)
	{
		NN_LOG_ERROR("Invalid message size, expected %d, got %d", header->headerSize + header->bodySize + 0x20, header->messSize);

		return false;
	}
	if (header->messSize > np::api::MESSAGE_MAX_SIZE)
	{
		NN_LOG_ERROR("Message size too large, expected %d, got %d", np::api::MESSAGE_MAX_SIZE, header->messSize);

		return false;
	}

	if (header->bodySize <= 0x20)
	{
		u8	b	= 0;
		u8* ptr = data + sizeof(nn::cec::CecMessageHeader);

		for (int i = 0; i < header->bodySize; i++)
		{
			b |= *ptr;
			ptr++;
		}

		if (b == 0)
		{
			NN_LOG_ERROR("Message body is empty");

			return false;
		}
	}

	return true;
}

nn::Result
WriteMessageToBox(nn::cec::MessageBox& messageBox, u8* data)
{
	// Data includes the message header
	nn::Result result;
	size_t	   readLength = 0;

	if (!IsValidMessage(data))
	{
		NN_LOG_ERROR("Invalid message");

		return np::api::ResultInvalidCecMessage();
	}

	nn::cec::CecMessageHeader* header = reinterpret_cast<nn::cec::CecMessageHeader*>(data);

	if (!header->recvDate.year)
		header->recvDate = nn::fnd::DateTime::GetNow().GetParameters();

	u32 maxMessages = messageBox.GetBoxMessageNumMax(nn::cec::CEC_BOXTYPE_INBOX);
	u32 maxBoxSize	= sizeof(nn::cec::CecBoxInfoHeader) + (maxMessages * sizeof(nn::cec::CecMessageHeader));

	for (int i = 0; i < messageBox.GetBoxMessageNum(nn::cec::CEC_BOXTYPE_INBOX); i++)
	{
		nn::cec::MessageId messageId = messageBox.GetMessageId(nn::cec::CEC_BOXTYPE_INBOX, i);

		if (0 == std::memcmp(messageId.GetBinary(), header->messageId, nn::cec::CEC_SIZEOF_MESSAGEID))
		{
			NN_LOG_INFO("Message already exists in inbox");

			return nn::ResultSuccess();
		}
	}

	if (messageBox.GetBoxMessageNum(nn::cec::CEC_BOXTYPE_INBOX) >= maxMessages)
	{
		NN_LOG_ERROR("Inbox is full");

		return np::api::ResultMessageBoxAtMaxCapacity();
	}

	if (messageBox.GetBoxMessageSizeMax(nn::cec::CEC_BOXTYPE_INBOX) < header->messSize)
	{
		NN_LOG_ERROR("Message too large for inbox");

		return np::api::ResultMessageTooLargeForBox();
	}

	if (messageBox.GetBoxSize(nn::cec::CEC_BOXTYPE_INBOX) + header->messSize > messageBox.GetBoxSizeMax(nn::cec::CEC_BOXTYPE_INBOX))
	{
		NN_LOG_ERROR("Inbox is full");

		return np::api::ResultMessageTooLargeForBox();
	}

	{
		nn::cec::MessageBoxInfo messageBoxInfo;
		np::util::GetCecMessageBoxInfo(messageBox.GetCurrentCecTitleId(), &messageBoxInfo);

		header->flagUnread = true;
		header->flagNew	   = true;

		result = nn::cec::CTR::detail::WriteMessageWithHmac(header->cecTitleId,
															nn::cec::CEC_BOXTYPE_INBOX,
															header->messageId,
															sizeof(header->messageId),
															data,
															header->messSize,
															reinterpret_cast<u8*>(messageBoxInfo.hmacKey));
		if (!result.IsSuccess())
		{
			NN_LOG_ERROR("Failed to write message");
			NN_DBG_PRINT_RESULT(result);

			return result;
		}

		// Check exists
		u8* buffer = reinterpret_cast<u8*>(std::malloc(header->messSize));
		result	   = nn::cec::CTR::detail::ReadMessage(header->cecTitleId,
												   nn::cec::CEC_BOXTYPE_INBOX,
												   header->messageId,
												   sizeof(header->messageId),
												   &readLength,
												   buffer,
												   header->messSize);
		std::free(buffer);
		if (result.IsFailure())
		{
			NN_LOG_ERROR("Failed to read message:");
			NN_DBG_PRINT_RESULT(result);

			return result;
		}
	}

	u8* boxData = reinterpret_cast<u8*>(std::malloc(maxBoxSize));
	result		= nn::cec::CTR::detail::OpenAndReadFile(boxData,
													maxBoxSize,
													&readLength,
													messageBox.GetCurrentCecTitleId(),
													nn::cec::CTR::FILETYPE_INBOX_INFO,
													nn::cec::CTR::FILEOPT_READ | nn::cec::CTR::FILEOPT_NOCHECK);
	if (result.IsFailure())
	{
		NN_LOG_ERROR("Failed to open and read inbox info:");
		NN_DBG_PRINT_RESULT(result);

		std::free(boxData);

		return result;
	}

	nn::cec::CecBoxInfoHeader* boxHeaderInfo = reinterpret_cast<nn::cec::CecBoxInfoHeader*>(boxData);
	nn::cec::CecMessageHeader* messages		 = reinterpret_cast<nn::cec::CecMessageHeader*>(boxData + sizeof(nn::cec::CecBoxInfoHeader));

	bool foundBox = false;
	for (int i = 0; i < boxHeaderInfo->messNum; i++)
	{
		if (0 == std::memcmp(messages[i].messageId, header->messageId, nn::cec::CEC_SIZEOF_MESSAGEID))
		{
			foundBox = true;
			break;
		}
	}

	if (!foundBox)
	{
		void* address = boxData + sizeof(nn::cec::CecBoxInfoHeader) + (boxHeaderInfo->messNum * sizeof(nn::cec::CecMessageHeader));
		std::memcpy(address, data, sizeof(nn::cec::CecMessageHeader));

		boxHeaderInfo->messNum++;
		boxHeaderInfo->boxInfoSize += sizeof(nn::cec::CecMessageHeader);
		boxHeaderInfo->boxSize += header->messSize;

		result = nn::cec::CTR::detail::OpenAndWriteFile(boxData,
														boxHeaderInfo->boxInfoSize,
														messageBox.GetCurrentCecTitleId(),
														nn::cec::CTR::FILETYPE_INBOX_INFO,
														nn::cec::CTR::FILEOPT_WRITE | nn::cec::CTR::FILEOPT_NOCHECK);
		if (result.IsFailure())
		{
			NN_LOG_ERROR("Failed to write inbox info:");
			NN_DBG_PRINT_RESULT(result);

			std::free(boxData);

			return result;
		}
	}

	std::free(boxData);

	{
		nn::cec::MessageBoxInfo messageBoxInfo;
		np::util::GetCecMessageBoxInfo(messageBox.GetCurrentCecTitleId(), &messageBoxInfo);

		messageBoxInfo.lastReceived = nn::fnd::DateTime::GetNow().GetParameters();
		messageBoxInfo.flag3		= true;

		result = nn::cec::CTR::detail::OpenAndWriteFile(reinterpret_cast<u8*>(&messageBoxInfo),
														sizeof(nn::cec::MessageBoxInfo),
														messageBox.GetCurrentCecTitleId(),
														nn::cec::CTR::FILETYPE_MESSAGE_BOX_INFO,
														nn::cec::CTR::FILEOPT_WRITE | nn::cec::CTR::FILEOPT_NOCHECK);
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

		nn::cec::MessageBox messageBox;
		result = np::util::OpenCecMessageBox(titleId, &messageBox);
		if (!result.IsSuccess())
		{
			NN_LOG_ERROR("Failed to open message box:");
			NN_DBG_PRINT_RESULT(result);

			PRINTF_DISPLAY1("Error downloading messages");

			s_WaitForDownloadEvent.Signal();
			s_WaitForDownloadEvent.ClearSignal();

			return;
		}

		u32 boxMessages = messageBox.GetBoxMessageNum(nn::cec::CEC_BOXTYPE_INBOX);
		u32 maxMessages = messageBox.GetBoxMessageNumMax(nn::cec::CEC_BOXTYPE_INBOX);

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
			u8 response[np::api::MESSAGE_MAX_SIZE];
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

				result = WriteMessageToBox(messageBox, response);
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

		messageBox.CloseMessageBox(true);  // Do not commit message box, it will overwrite our update.
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
