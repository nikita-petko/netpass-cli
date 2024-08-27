#include <np/api.h>
#include <np/config.h>
#include <np/graphics.h>
#include <np/http.h>
#include <np/memory.h>
#include <np/util.h>

#include <np/log.h>

#include "api/api_Urls.h"

namespace {

nn::Result
UpdateOutbox(u8* messageBuffer)
{
	nn::Result result;
	size_t	   readLength = 0;

	NN_LOG_INFO("Updating outbox, message buffer: %p", messageBuffer);

	// Sanity check for message validity
	if (!np::util::IsValidCecMessage(messageBuffer))
	{
		NN_LOG_ERROR("Invalid message");

		return np::api::ResultInvalidCecMessage();
	}

	nn::cec::CecMessageHeader* messageHeader = reinterpret_cast<nn::cec::CecMessageHeader*>(messageBuffer);

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
											  nn::cec::FILETYPE_OUTBOX_INFO,
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
											  nn::cec::FILETYPE_OUTBOX_INFO,
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

	u32 messageIndex = 0;
	for (int i = 0; i < boxInfoHeader->messNum; i++)
	{
		if (0 == std::memcmp(boxMessages[i].messageId, messageHeader->messageId, nn::cec::CEC_SIZEOF_MESSAGEID))
		{
			messageIndex = i;

			break;
		}
	}

	if (messageIndex < 0)
	{
		NN_LOG_ERROR("Message not found in outbox");

		std::free(boxBuffer);

		return np::api::ResultMessageNotFound();
	}

	// Update the message
	std::memcpy(&boxMessages[messageIndex], messageHeader, sizeof(nn::cec::CecMessageHeader));

	// Write the updated message box
	result = nn::cec::detail::OpenAndWriteFile(boxBuffer,
											   boxInfoHeader->boxInfoSize,
											   messageHeader->cecTitleId,
											   nn::cec::FILETYPE_OUTBOX_INFO,
											   nn::cec::FILEOPT_WRITE | nn::cec::FILEOPT_NOCHECK);
	if (result.IsFailure())
	{
		NN_LOG_ERROR("Failed to write outbox info:");
		NN_DBG_PRINT_RESULT(result);

		std::free(boxBuffer);

		return result;
	}

	std::free(boxBuffer);

	// Fetch the Hmac and update the message
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

	result = nn::cec::detail::WriteMessageWithHmac(messageHeader->cecTitleId,
												   nn::cec::CEC_BOXTYPE_OUTBOX,
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

	return nn::ResultSuccess();
}

}  // namespace

namespace np { namespace api {

	nn::Result UploadOutboxes(void)
	{
		NN_LOG_INFO("Uploading outboxes to the server");

		nn::Result				result;
		nn::cec::MessageBoxList messageBoxList;

		result = np::util::GetCecMessageBoxList(&messageBoxList);
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
				NN_DBG_PRINT_RESULT(result);

				return ResultMessageBoxListUploadHttpError();
			}

			NN_LOG_INFO("Uploaded message box list to the server");
		}

		NN_LOG_INFO("Uploading outboxes (total : %d)", messageBoxList.DirNum);
		for (int i = 0; i < messageBoxList.DirNum; i++)
		{
			const u8* titleIdStr = messageBoxList.DirName[i];

			PRINTF_DISPLAY1("Uploading outbox for title %s", titleIdStr);

			nn::cec::TitleId titleId = std::strtoul(reinterpret_cast<const char*>(titleIdStr), NULL, 16);

			NN_LOG_INFO("Opening message box for title %s", titleIdStr);

			nn::cec::MessageBox messageBox;
			result = np::util::OpenCecMessageBox(titleId, &messageBox);
			if (!result.IsSuccess())
			{
				NN_LOG_ERROR("Failed to open message box:");
				NN_DBG_PRINT_RESULT(result);

				return result;
			}

			size_t titleNameSize;
			result = np::util::GetTitleName(titleId, NULL, &titleNameSize);
			if (!result.IsSuccess())
			{
				NN_LOG_ERROR("Failed to get title name size:");
				NN_DBG_PRINT_RESULT(result);

				return result;
			}

			char16* titleName = reinterpret_cast<char16*>(std::malloc(titleNameSize));
			result			  = np::util::GetTitleName(titleId, titleName, &titleNameSize);
			if (!result.IsSuccess())
			{
				NN_LOG_ERROR("Failed to get title name:");
				NN_DBG_PRINT_RESULT(result);

				std::free(titleName);

				return result;
			}

			size_t titleBase64Size = 0;
			np::util::Base64Encode(NULL, &titleBase64Size, titleName, titleNameSize);

			char8* titleBase64 = reinterpret_cast<char8*>(std::malloc(titleBase64Size));
			np::util::Base64Encode(titleBase64, &titleBase64Size, titleName, titleNameSize);

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
										MEM_SIZEOF_ARRAY(additionalHeaders));

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
						result = ::UpdateOutbox(messageBuffer);
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
			messageBox.CloseMessageBox(true);
		}

		return result;
	}

}}	// namespace np::api
