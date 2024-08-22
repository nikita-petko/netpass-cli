#include <np/api.h>
#include <np/config.h>
#include <np/graphics.h>
#include <np/http.h>
#include <np/memory.h>
#include <np/util.h>

#include <np/log.h>

#include "api/api_Urls.h"

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

			PRINTF_DISPLAY1("Uploading outbox for title %s", titleIdStr, i + 1, messageBoxList.DirNum);

			nn::cec::TitleId titleId = std::strtol(reinterpret_cast<const char*>(titleIdStr), NULL, 16);

			NN_LOG_INFO("Opening message box for title %s", titleIdStr);

			nn::cec::MessageBox messageBox;
			result = np::util::OpenCecMessageBox(titleId, &messageBox);
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
