#ifndef NP_UTIL_UTIL_BOXUTIL_H_
#define NP_UTIL_UTIL_BOXUTIL_H_

#include <nn/cec.h>

namespace np { namespace util {

	/*!
		@brief The maximum size of a message in bytes.
	*/
	static const size_t CEC_MESSAGE_MAX_SIZE = sizeof(nn::cec::CecMessageHeader) + nn::cec::Message::MESSAGE_BODY_SIZE_MAX;

	/*!
		@brief Get the message box header for the specified title ID.

		@param[in] titleId The title ID to get the message box header for.
		@param[out] pOut The message box header to fill in.

		@return The result of the operation.
	*/
	nn::Result GetCecMessageBoxInfo(nn::cec::TitleId titleId, nn::cec::MessageBoxInfo* pOut);

	/*!
		@brief Get the message box list.

		@param[out] pOut The message box list to fill in.

		@return The result of the operation.
	*/
	nn::Result GetCecMessageBoxList(nn::cec::MessageBoxList* pOut);

	/*!
		@brief Get the message box header for the specific box.

		@param[in] titleId The title ID to get the message box header for.
		@param[in] boxType The box type to get the message box header for.
		@param[out] pOut The message box header to fill in.

		@return The result of the operation.
	*/
	nn::Result GetCecMessageBoxHeader(nn::cec::TitleId titleId, nn::cec::CecBoxType boxType, nn::cec::CecBoxInfoHeader* pOut);

	/*!
		@brief Open the message box for the specified title ID.
		@note This prefetches the message box private ID (if it exists) and opens the message box.
		@warning This function may be removed when we move to directly opening CEC files instead of using the UTIL.

		@param[in] titleId The title ID to open the message box for.
		@param[out] pOut The message box to fill in.

		@return The result of the operation.
	*/
	nn::Result OpenCecMessageBox(nn::cec::TitleId titleId, nn::cec::MessageBox* pOut);

	/*!
		@brief Gets the title name for the specified title ID.
		@note Set the pTitleName to NULL to get the size of the title name buffer.

		@param[in] titleId The title ID to get the title name for.
		@param[out] pTitleName The title name to fill in.
		@param[in,out] titleNameSize The size of the title name buffer.

		@return The result of the operation.
	*/
	nn::Result GetTitleName(nn::cec::TitleId titleId, char16* pTitleName, size_t* titleNameSize);

	/*!
		@brief Validates the specified buffer as a nn::cec::Message.

		@param[in] pMessage The message buffer to validate.

		@return True if the message is valid, false otherwise.
	*/
	bool IsValidCecMessage(u8* pMessage);

}}	// namespace np::util

#endif	// NP_UTIL_UTIL_BOXUTIL_H_
