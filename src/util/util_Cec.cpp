#include <np/util.h>

#include <np/log.h>

namespace np { namespace util {

	nn::Result GetCecMessageBoxInfo(nn::cec::TitleId titleId, nn::cec::MessageBoxInfo* messageBoxInfo)
	{
		NN_LOG_INFO("Getting message box info for title 0x%08X", titleId);

		nn::Result result;

		size_t readLength = 0;
		return nn::cec::CTR::detail::OpenAndReadFile(reinterpret_cast<u8*>(messageBoxInfo),
													 sizeof(nn::cec::MessageBoxInfo),
													 &readLength,
													 titleId,
													 nn::cec::CTR::FILETYPE_MESSAGE_BOX_INFO,
													 nn::cec::CTR::FILEOPT_READ | nn::cec::CTR::FILEOPT_NOCHECK);
	}

	nn::Result GetCecMessageBoxList(nn::cec::MessageBoxList* messageBoxList)
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

	nn::Result GetCecMessageBoxHeader(nn::cec::TitleId titleId, nn::cec::CecBoxType boxType, nn::cec::CecBoxInfoHeader* pOut)
	{
		NN_LOG_INFO("Getting message box header for title 0x%08X", titleId);

		nn::Result result;

		size_t readLength = 0;
		if (boxType == nn::cec::CEC_BOXTYPE_INBOX)
		{
			return nn::cec::CTR::detail::OpenAndReadFile(reinterpret_cast<u8*>(pOut),
														 nn::cec::CEC_SIZEOF_BOXINFO_HEADER,
														 &readLength,
														 titleId,
														 nn::cec::CTR::FILETYPE_INBOX_INFO,
														 nn::cec::CTR::FILEOPT_READ | nn::cec::CTR::FILEOPT_NOCHECK);
		}
		else
		{
			return nn::cec::CTR::detail::OpenAndReadFile(reinterpret_cast<u8*>(pOut),
														 nn::cec::CEC_SIZEOF_BOXINFO_HEADER,
														 &readLength,
														 titleId,
														 nn::cec::CTR::FILETYPE_OUTBOX_INFO,
														 nn::cec::CTR::FILEOPT_READ | nn::cec::CTR::FILEOPT_NOCHECK);
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

}}	// namespace np::util
