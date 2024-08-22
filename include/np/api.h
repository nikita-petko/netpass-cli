#ifndef NP_API_H_
#define NP_API_H_

#include <nn/cec.h>
#include <nn/fnd.h>
#include <nn/types.h>

namespace np { namespace api {

	/*!
		@brief The maximum size of a message in bytes.
	*/
	static const size_t MESSAGE_MAX_SIZE = sizeof(nn::cec::CecMessageHeader) + nn::cec::Message::MESSAGE_BODY_SIZE_MAX;

	/*!
		@brief Result returned when the message box list has not been uploaded.
	*/
	NN_DEFINE_RESULT_CONST(ResultMessageBoxListNotUploaded,
						   nn::Result::LEVEL_PERMANENT,
						   nn::Result::SUMMARY_INVALID_STATE,
						   nn::Result::MODULE_APPLICATION,
						   nn::Result::DESCRIPTION_NOT_INITIALIZED);

	/*!
		@brief Result returned when the system has no registered message boxes.
	*/
	NN_DEFINE_RESULT_CONST(ResultNoMessageBoxes,  // Non fatal, just warn the user and exit
						   nn::Result::LEVEL_PERMANENT,
						   nn::Result::SUMMARY_INVALID_STATE,
						   nn::Result::MODULE_APPLICATION,
						   nn::Result::DESCRIPTION_NO_DATA);

	/*!
		@brief Result returned when a http error occurs while uploading the message box list.
	*/
	NN_DEFINE_RESULT_CONST(ResultMessageBoxListUploadHttpError,
						   nn::Result::LEVEL_PERMANENT,
						   nn::Result::SUMMARY_INTERNAL,
						   nn::Result::MODULE_APPLICATION,
						   nn::Result::DESCRIPTION_INVALID_RESULT_VALUE);

	/*!
		@brief Result returned when there are available boxes on the system but they are all disabled in the settings.
	*/
	NN_DEFINE_RESULT_CONST(ResultAllMessageBoxesDisabled,
						   nn::Result::LEVEL_STATUS,
						   nn::Result::SUMMARY_INTERNAL,
						   nn::Result::MODULE_APPLICATION,
						   nn::Result::DESCRIPTION_NO_DATA);

	/*!
		@brief Result returned when the message is invalid.
	*/
	NN_DEFINE_RESULT_CONST(ResultInvalidCecMessage,
						   nn::Result::LEVEL_PERMANENT,
						   nn::Result::SUMMARY_INVALID_ARGUMENT,
						   nn::Result::MODULE_APPLICATION,
						   nn::Result::DESCRIPTION_INVALID_SELECTION);

	/*!
		@brief Result returned when the message box is at max capacity.
	*/
	NN_DEFINE_RESULT_CONST(ResultMessageBoxAtMaxCapacity,
						   nn::Result::LEVEL_PERMANENT,
						   nn::Result::SUMMARY_OUT_OF_RESOURCE,
						   nn::Result::MODULE_APPLICATION,
						   nn::Result::DESCRIPTION_INVALID_SIZE);

	/*!
		@brief Result returned when the message is too large for the box.
	*/
	NN_DEFINE_RESULT_CONST(ResultMessageTooLargeForBox,
						   nn::Result::LEVEL_PERMANENT,
						   nn::Result::SUMMARY_INVALID_ARGUMENT,
						   nn::Result::MODULE_APPLICATION,
						   nn::Result::DESCRIPTION_INVALID_SIZE);

	/*!
		@brief Result returned when no suitable pass is found.
	*/
	NN_DEFINE_RESULT_CONST(ResultNoSuitablePassFound,
						   nn::Result::LEVEL_PERMANENT,
						   nn::Result::SUMMARY_NOT_FOUND,
						   nn::Result::MODULE_APPLICATION,
						   nn::Result::DESCRIPTION_NO_DATA);

	/*!
		@brief The current location of the API.
	*/
	extern s8 s_Location;

	/*!
		@brief Get the message box header for the specified title ID.

		@param[in] titleId The title ID to get the message box header for.
		@param[out] pOut The message box header to fill in.

		@return The result of the operation.
	*/
	nn::Result GetMessageBoxInfo(nn::cec::TitleId titleId, nn::cec::MessageBoxInfo* pOut);

	/*!
		@brief Get the message box list.

		@param[out] pOut The message box list to fill in.

		@return The result of the operation.
	*/
	nn::Result GetMessageBoxList(nn::cec::MessageBoxList* pOut);

	/*!
		@brief Get the message box header for the specific box.

		@param[in] titleId The title ID to get the message box header for.
		@param[in] boxType The box type to get the message box header for.
		@param[out] pOut The message box header to fill in.

		@return The result of the operation.
	*/
	nn::Result GetMessageBoxHeader(nn::cec::TitleId titleId, nn::cec::CecBoxType boxType, nn::cec::CecBoxInfoHeader* pOut);

	/*!
		@brief Open the message box for the specified title ID.
		@note This prefetches the message box private ID (if it exists) and opens the message box.
		@warning This function may be removed when we move to directly opening CEC files instead of using the API.

		@param[in] titleId The title ID to open the message box for.
		@param[out] pOut The message box to fill in.

		@return The result of the operation.
	*/
	nn::Result OpenMessageBox(nn::cec::TitleId titleId, nn::cec::MessageBox* pOut);

	/*!
		@brief Starts the background thread for downloading messages.
	*/
	nn::Result StartBackgroundThread(void);

	/*!
		@brief Finalizes the API.
	*/
	void Finalize(void);

	/*!
		@brief Get the current location from the API.

		@param[out] location  The location to fill in.

		@return The result of the operation.
	*/
	nn::Result GetLocation(s8* location);

	/*!
		@brief Set the current location in the API.

		@param  location  The location to set.

		@return The result of the operation.
	*/
	nn::Result SetLocation(s8 location);

	/*!
		@brief Uploads all of the current outboxes to the server.

		@return The result of the operation.
	*/
	nn::Result UploadOutboxes(void);

	/*!
		@brief Requests a pass for the specified title ID be placed into our next inbox.
		@warning This does not guarantee that the pass will be placed into the inbox, only that it will be requested.

		@param[in] titleId The title ID to request a pass for.

		@return The result of the operation.
	*/
	nn::Result RequestPass(nn::cec::TitleId titleId);

	/*!
		@brief Triggers an immediate download of message boxes.
	*/
	void TriggerInboxDownload(void);

	/*!
		@brief Waits for the inbox download to complete.
		@warning This function will block until the inbox download is complete, so be careful when calling it.
	*/
	void WaitForInboxDownloadComplete(void);
}}	// namespace np::api

#endif	// NP_API_H_
