#ifndef NP_HTTP_HTTP_RESULT_H_
#define NP_HTTP_HTTP_RESULT_H_

#include <nn/Result.h>
#include <nn/http.h>

namespace np { namespace http {

	/*!
		@brief  Result returned when the HTTP post buffer is could not be allocated.
	*/
	NN_DEFINE_RESULT_CONST(ResultHttpPostBufferAllocationFailed,
						   nn::Result::LEVEL_PERMANENT,
						   nn::Result::SUMMARY_OUT_OF_RESOURCE,
						   nn::Result::MODULE_NN_HTTP,
						   nn::Result::DESCRIPTION_OUT_OF_MEMORY);

	/*!
		@brief  Result returned when the HTTP library is not initialized.
	*/
	NN_DEFINE_RESULT_CONST(ResultHttpNotInitialized,
						   nn::Result::LEVEL_PERMANENT,
						   nn::Result::SUMMARY_INTERNAL,
						   nn::Result::MODULE_APPLICATION,
						   nn::Result::DESCRIPTION_NOT_INITIALIZED);

	/*!
		@brief  Result returned when the HTTP request failed.
	*/
	NN_DEFINE_RESULT_CONST(ResultHttpRequestFailed,
						   nn::Result::LEVEL_PERMANENT,
						   nn::Result::SUMMARY_INTERNAL,
						   nn::Result::MODULE_NN_HTTP,
						   nn::Result::DESCRIPTION_INVALID_RESULT_VALUE);

}}	// namespace np::http

#endif	// NP_HTTP_HTTP_RESULT_H_
