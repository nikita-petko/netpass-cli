#ifndef NP_API_API_RESULT_H_
#define NP_API_API_RESULT_H_

#include <nn/Result.h>

namespace np { namespace api {

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

}}	// namespace np::api

#endif	// NP_API_API_RESULT_H_
