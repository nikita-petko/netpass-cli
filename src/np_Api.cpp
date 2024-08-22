#include <np/api.h>
#include <np/base64.h>
#include <np/config.h>
#include <np/graphics.h>
#include <np/http.h>
#include <np/memory.h>
#include <np/scene/scene_SceneManager.h>

#include <nn/cec.h>

#include <np/log_Macros.h>

#define API_BASE_URL "http://netpass-proxy.service.vmminfra.net"

#define API_GET_LOCATION	 "/location/current"
#define API_URL_GET_LOCATION API_BASE_URL API_GET_LOCATION

#define API_SET_LOCATION	 "/location/%d/enter"
#define API_URL_SET_LOCATION API_BASE_URL API_SET_LOCATION

#define API_UPLOAD_MESSAGE_BOX_LIST		"/outbox/mboxlist"
#define API_URL_UPLOAD_MESSAGE_BOX_LIST API_BASE_URL API_UPLOAD_MESSAGE_BOX_LIST

#define API_DELETE_OUTBOX	  "/outbox/%s"
#define API_URL_DELETE_OUTBOX API_BASE_URL API_DELETE_OUTBOX

#define API_UPLOAD_OUTBOX	  "/outbox/upload"
#define API_URL_UPLOAD_OUTBOX API_BASE_URL API_UPLOAD_OUTBOX

#define API_POP_MESSAGE		"/inbox/%s/pop"
#define API_URL_POP_MESSAGE API_BASE_URL API_POP_MESSAGE

#define API_REQUEST_PASSES	   "/pass/title_id/%08X"
#define API_URL_REQUEST_PASSES API_BASE_URL API_REQUEST_PASSES

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
		np::api::GetMessageBoxInfo(messageBox.GetCurrentCecTitleId(), &messageBoxInfo);

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
		np::api::GetMessageBoxInfo(messageBox.GetCurrentCecTitleId(), &messageBoxInfo);

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
	result = np::api::GetMessageBoxList(&messageBoxList);
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
		result = np::api::OpenMessageBox(titleId, &messageBox);
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

	s8 s_Location = -1;

	nn::Result StartBackgroundThread(void)
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

	void Finalize(void)
	{
		NN_LOG_INFO("Finalizing np::api");

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

	nn::Result GetLocation(s8* location)
	{
		NN_LOG_INFO("Getting location from API");

		nn::Result result;

		s32 statusCode = 0;

		u8 response;
		result = np::http::Get(API_URL_GET_LOCATION, &response, sizeof(response), &statusCode);

		*location = static_cast<s8>(response);

		NN_LOG_INFO("Got location: %d, statusCode: %d", *location, statusCode);

		if (result == nn::http::ResultBodyBufShortage())
			// This will always be the case.
			result = nn::ResultSuccess();

		if (statusCode != 200)
		{
			NN_LOG_WARN("Status code was 204, or an error, setting location to -1 to be safe");

			*location = -1;
		}

		return result;
	}

	nn::Result SetLocation(s8 location)
	{
		NN_LOG_INFO("Setting location to %d", location);

		nn::Result result;

		s32 upcastedLocation = static_cast<s32>(location);

		char url[128];
		nn::nstd::TSNPrintf(url, sizeof(url), API_URL_SET_LOCATION, upcastedLocation);

		s32 statusCode = 0;
		result		   = np::http::Put(url, NULL, 0, &statusCode);
		// This API will 409 if the outboxes were not uploaded for the mac address

		if (statusCode == 409)
		{
			NN_LOG_WARN("HTTP request failed with status code %d, did you upload the outboxes?", statusCode);

			return ResultMessageBoxListNotUploaded();
		}

		np::config::GetConfiguration().last_location = location;
		np::config::FlushConfiguration();

		PRINTF_DISPLAY1("Location set to %d", location);

		return result;
	}

	nn::Result RequestPass(nn::cec::TitleId titleId)
	{
		NN_LOG_INFO("Requesting pass for title %d", titleId);

		nn::Result result;

		char8 url[sizeof(API_URL_REQUEST_PASSES) + 16];
		nn::nstd::TSNPrintf(url, sizeof(url), API_URL_REQUEST_PASSES, titleId);

		s32 statusCode = 0;
		result		   = np::http::Put(url, NULL, 0, &statusCode);

		if (result.IsFailure())
		{
			NN_LOG_ERROR("Failed to request pass:");
			NN_DBG_PRINT_RESULT(result);

			return result;
		}

		if (statusCode == 404)
		{
			NN_LOG_WARN("No suitable pass found");

			return ResultNoSuitablePassFound();
		}

		return result;
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

	nn::Result GetMessageBoxInfo(nn::cec::TitleId titleId, nn::cec::MessageBoxInfo* messageBoxInfo)
	{
		NN_LOG_INFO("Getting message box info for title %d", titleId);

		nn::Result result;

		size_t readLength = 0;
		return nn::cec::CTR::detail::OpenAndReadFile(reinterpret_cast<u8*>(messageBoxInfo),
													 sizeof(nn::cec::MessageBoxInfo),
													 &readLength,
													 titleId,
													 nn::cec::CTR::FILETYPE_MESSAGE_BOX_INFO,
													 nn::cec::CTR::FILEOPT_READ | nn::cec::CTR::FILEOPT_NOCHECK);
	}

	nn::Result GetMessageBoxList(nn::cec::MessageBoxList* messageBoxList)
	{
		NN_LOG_INFO("Getting message box list");

		nn::Result result;

		size_t readLength = 0;
		return nn::cec::CTR::detail::OpenAndReadFile(reinterpret_cast<u8*>(messageBoxList),
													 sizeof(nn::cec::MessageBoxList),
													 &readLength,
													 0,
													 nn::cec::CTR::FILETYPE_MESSAGE_BOX_LIST,
													 nn::cec::CTR::FILEOPT_READ | nn::cec::CTR::FILEOPT_NOCHECK);
	}

	nn::Result GetMessageBoxHeader(nn::cec::TitleId titleId, nn::cec::CecBoxType boxType, nn::cec::CecBoxInfoHeader* pOut)
	{
		NN_LOG_INFO("Getting message box header for title %d", titleId);

		nn::Result result;

		size_t readLength = 0;
		if (boxType == nn::cec::CEC_BOXTYPE_INBOX)
		{
			return nn::cec::CTR::detail::OpenAndReadFile(reinterpret_cast<u8*>(pOut),
														 sizeof(nn::cec::CecBoxInfoHeader),
														 &readLength,
														 titleId,
														 nn::cec::CTR::FILETYPE_INBOX_INFO,
														 nn::cec::CTR::FILEOPT_READ | nn::cec::CTR::FILEOPT_NOCHECK);
		}
		else
		{
			return nn::cec::CTR::detail::OpenAndReadFile(reinterpret_cast<u8*>(pOut),
														 sizeof(nn::cec::CecBoxInfoHeader),
														 &readLength,
														 titleId,
														 nn::cec::CTR::FILETYPE_OUTBOX_INFO,
														 nn::cec::CTR::FILEOPT_READ | nn::cec::CTR::FILEOPT_NOCHECK);
		}
	}

	nn::Result OpenMessageBox(nn::cec::TitleId titleId, nn::cec::MessageBox* messageBox)
	{
		NN_LOG_INFO("Opening message box for title %d", titleId);

		nn::Result result;

		nn::cec::MessageBoxInfo messageBoxInfo;
		result = GetMessageBoxInfo(titleId, &messageBoxInfo);
		if (!result.IsSuccess())
		{
			NN_LOG_ERROR("Failed to get message box info:");
			NN_DBG_PRINT_RESULT(result);

			return result;
		}

		return messageBox->OpenMessageBox(titleId, messageBoxInfo.privateId);
	}

	nn::Result UploadOutboxes(void)
	{
		NN_LOG_INFO("Uploading outboxes to the server");

		nn::Result				result;
		nn::cec::MessageBoxList messageBoxList;

		result = GetMessageBoxList(&messageBoxList);
		if (!result.IsSuccess())
		{
			NN_LOG_ERROR("Failed to open and read message box list:");
			NN_DBG_PRINT_RESULT(result);

			return result;
		}

		if (messageBoxList.DirNum == 0)
		{
			NN_LOG_INFO("No outboxes to upload");

			return ResultNoMessageBoxes();
		}

		np::config::ClearIgnoredTitles(&messageBoxList);

		if (messageBoxList.DirNum == 0)
		{
			NN_LOG_INFO("All message boxes are disabled");

			return ResultAllMessageBoxesDisabled();
		}

		NN_LOG_INFO("Uploading message box list to the server");
		{
			s32 statusCode = 0;
			result		   = np::http::Post(API_URL_UPLOAD_MESSAGE_BOX_LIST,
									reinterpret_cast<u8*>(&messageBoxList),
									sizeof(nn::cec::MessageBoxList),
									&statusCode);
			if (!result.IsSuccess())
			{
				NN_LOG_ERROR("Failed to upload message box list:");
				NN_DBG_PRINT_RESULT(result);

				return result;
			}

			if (statusCode != 200)
			{
				NN_LOG_ERROR("HTTP request failed with status code %d", statusCode);

				return ResultMessageBoxListUploadHttpError();
			}

			NN_LOG_INFO("Uploaded message box list to the server");
		}

		NN_LOG_INFO("Uploading outboxes (total : %d)", messageBoxList.DirNum);
		for (int i = 0; i < messageBoxList.DirNum; i++)
		{
			const u8* titleIdStr = messageBoxList.DirName[i];

			PRINTF_DISPLAY1("Uploading outbox for title %s", titleIdStr, i + 1, messageBoxList.DirNum);

			nn::cec::TitleId titleId = std::strtol(reinterpret_cast<const char*>(titleIdStr), NULL, 16);

			NN_LOG_INFO("Opening message box for title %s", titleIdStr);

			nn::cec::MessageBox messageBox;
			result = OpenMessageBox(titleId, &messageBox);
			if (!result.IsSuccess())
			{
				NN_LOG_ERROR("Failed to open message box:");
				NN_DBG_PRINT_RESULT(result);

				return result;
			}

			u32		titleNameSize = messageBox.GetMessageBoxDataSize(nn::cec::CTR::BOXDATA_TYPE_NAME_1);
			char16* titleName	  = reinterpret_cast<char16*>(std::malloc(titleNameSize));
			result				  = messageBox.GetMessageBoxData(nn::cec::CTR::BOXDATA_TYPE_NAME_1, titleName, titleNameSize);
			if (!result.IsSuccess())
			{
				NN_LOG_ERROR("Failed to get title name:");
				NN_DBG_PRINT_RESULT(result);

				std::free(titleName);

				return result;
			}

			size_t titleBase64Size = 0;
			np::Base64Encode(NULL, &titleBase64Size, titleName, titleNameSize);

			char8* titleBase64 = reinterpret_cast<char8*>(std::malloc(titleBase64Size));
			np::Base64Encode(titleBase64, &titleBase64Size, titleName, titleNameSize);

			std::free(titleName);

			for (int j = 0; j < messageBox.GetBoxMessageNum(nn::cec::CEC_BOXTYPE_OUTBOX); j++)
			{
				nn::cec::Message   message;
				u32				   messageSize	 = messageBox.GetMessageSize(nn::cec::CEC_BOXTYPE_OUTBOX, j);
				nn::cec::MessageId messageId	 = messageBox.GetMessageId(nn::cec::CEC_BOXTYPE_OUTBOX, j);
				u8*				   messageBuffer = reinterpret_cast<u8*>(std::malloc(messageSize));

				NN_LOG_INFO("Reading message %d from outbox %d, size: %d", messageId.GetString(), j, messageSize);

				result = messageBox.ReadMessage(message, messageBuffer, messageSize, nn::cec::CEC_BOXTYPE_OUTBOX, messageId);
				if (!result.IsSuccess())
				{
					NN_LOG_ERROR("Failed to read message:");
					NN_DBG_PRINT_RESULT(result);

					// Delete the message box (from the server)
					char url[sizeof(API_BASE_URL) + sizeof("/outbox/") + 16];
					nn::nstd::TSNPrintf(url, sizeof(url), API_URL_DELETE_OUTBOX, titleIdStr);

					PRINTF_DISPLAY1("Deleting outbox %d", titleIdStr);

					np::http::Delete(url);

					std::free(titleBase64);
					std::free(messageBuffer);

					return result;
				}

				NN_LOG_INFO("Uploading outbox %d", messageId.GetString());

				np::http::HttpHeader additionalHeaders[] = {{"3ds-title-name", reinterpret_cast<const char*>(titleBase64)}};

				// Response is a byte with the new sendCount
				u8	sendCount  = 0;
				s32 statusCode = 0;
				result		   = np::http::Post(API_URL_UPLOAD_OUTBOX,
										messageBuffer,
										messageSize,
										&statusCode,
										&sendCount,
										sizeof(sendCount),
										additionalHeaders,
										SIZEOF_ARRAY(additionalHeaders));

				if (!result.IsSuccess() && result != nn::http::ResultBodyBufShortage())
				{
					NN_LOG_ERROR("Failed to upload outbox:");
					NN_DBG_PRINT_RESULT(result);

					std::free(titleBase64);
					std::free(messageBuffer);

					return result;
				}

				NN_LOG_INFO("Uploaded for outbox %d, statusCode: %d, new sendCount: %d", messageId.GetString(), statusCode, sendCount);

				if (statusCode == 200)
				{
					NN_LOG_INFO("Uploaded outbox %d, updating to new sendCount %d", messageId.GetString(), sendCount);

					if (sendCount < message.GetSendCount())
					{
						message.SetSendCount(sendCount);
						result = messageBox.WriteMessage(message, nn::cec::CEC_BOXTYPE_OUTBOX, messageId);
						if (!result.IsSuccess())
						{
							NN_LOG_ERROR("Failed to update sendCount:");
							NN_DBG_PRINT_RESULT(result);

							std::free(titleBase64);
							std::free(messageBuffer);

							return result;
						}
					}
				}

				std::free(messageBuffer);
			}

			NN_LOG_INFO("Closing message box for title %s", titleIdStr);

			std::free(titleBase64);
			messageBox.CloseMessageBox();
		}

		return result;
	}
}}	// namespace np::api
