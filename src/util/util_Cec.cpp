#include <np/util.h>

#include <np/log.h>

namespace np { namespace util {

	nn::Result GetCecMessageBoxInfo(nn::cec::TitleId titleId, nn::cec::MessageBoxInfo* messageBoxInfo)
	{
		NN_LOG_INFO("Getting message box info for title 0x%08X", titleId);

		nn::Result result;

		size_t readLength = 0;
		return nn::cec::detail::OpenAndReadFile(reinterpret_cast<u8*>(messageBoxInfo),
												sizeof(nn::cec::MessageBoxInfo),
												&readLength,
												titleId,
												nn::cec::FILETYPE_MESSAGE_BOX_INFO,
												nn::cec::FILEOPT_READ | nn::cec::FILEOPT_NOCHECK);
	}

	nn::Result GetCecMessageBoxList(nn::cec::MessageBoxList* messageBoxList)
	{
		NN_LOG_INFO("Getting message box list");

		nn::Result result;

		size_t readLength = 0;
		return nn::cec::detail::OpenAndReadFile(reinterpret_cast<u8*>(messageBoxList),
												sizeof(nn::cec::MessageBoxList),
												&readLength,
												0,
												nn::cec::FILETYPE_MESSAGE_BOX_LIST,
												nn::cec::FILEOPT_READ | nn::cec::FILEOPT_NOCHECK);
	}

	nn::Result GetCecMessageBoxHeader(nn::cec::TitleId titleId, nn::cec::CecBoxType boxType, nn::cec::CecBoxInfoHeader* pOut)
	{
		NN_LOG_INFO("Getting message box header for title 0x%08X", titleId);

		nn::Result result;

		size_t readLength = 0;
		if (boxType == nn::cec::CEC_BOXTYPE_INBOX)
		{
			return nn::cec::detail::OpenAndReadFile(reinterpret_cast<u8*>(pOut),
													nn::cec::CEC_SIZEOF_BOXINFO_HEADER,
													&readLength,
													titleId,
													nn::cec::FILETYPE_INBOX_INFO,
													nn::cec::FILEOPT_READ | nn::cec::FILEOPT_NOCHECK);
		}
		else
		{
			return nn::cec::detail::OpenAndReadFile(reinterpret_cast<u8*>(pOut),
													nn::cec::CEC_SIZEOF_BOXINFO_HEADER,
													&readLength,
													titleId,
													nn::cec::FILETYPE_OUTBOX_INFO,
													nn::cec::FILEOPT_READ | nn::cec::FILEOPT_NOCHECK);
		}
	}

	nn::Result OpenCecMessageBox(nn::cec::TitleId titleId, nn::cec::MessageBox* messageBox)
	{
		NN_LOG_INFO("Opening message box for title 0x%08X", titleId);

		nn::Result result;

		nn::cec::MessageBoxInfo messageBoxInfo;
		result = GetCecMessageBoxInfo(titleId, &messageBoxInfo);
		if (!result.IsSuccess())
		{
			NN_LOG_ERROR("Failed to get message box info:");
			NN_DBG_PRINT_RESULT(result);

			return result;
		}

		return messageBox->OpenMessageBox(titleId, messageBoxInfo.privateId);
	}

	nn::Result GetTitleName(nn::cec::TitleId titleId, char16* pTitleName, size_t* titleNameSize)
	{
		NN_LOG_INFO("Getting title name for title 0x%08X", titleId);

		nn::Result result;

		result = nn::cec::detail::Open(titleId, nn::cec::FILETYPE_BOXDATA_NAME_1, nn::cec::FILEOPT_READ, titleNameSize);
		if (!result.IsSuccess())
		{
			NN_LOG_ERROR("Failed to open file for title name:");
			NN_DBG_PRINT_RESULT(result);

			return result;
		}

		if (pTitleName)
		{
			size_t readLength = 0;
			result			  = nn::cec::detail::OpenAndReadFile(reinterpret_cast<u8*>(pTitleName),
														 *titleNameSize,
														 &readLength,
														 titleId,
														 nn::cec::FILETYPE_BOXDATA_NAME_1,
														 nn::cec::FILEOPT_READ);
			if (!result.IsSuccess())
			{
				NN_LOG_ERROR("Failed to read title name:");
				NN_DBG_PRINT_RESULT(result);

				return result;
			}
		}

		return nn::ResultSuccess();
	}

	bool IsValidCecMessage(u8* messageBuffer)
	{
		nn::cec::CecMessageHeader* header = reinterpret_cast<nn::cec::CecMessageHeader*>(messageBuffer);

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
		if (header->messSize > np::util::CEC_MESSAGE_MAX_SIZE)
		{
			NN_LOG_ERROR("Message size too large, expected %d, got %d", np::util::CEC_MESSAGE_MAX_SIZE, header->messSize);

			return false;
		}

		if (header->bodySize <= 0x20)
		{
			u8	b	= 0;
			u8* ptr = messageBuffer + sizeof(nn::cec::CecMessageHeader);

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

}}	// namespace np::util
