#ifndef NP_UTIL_UTIL_BASE64_H_
#define NP_UTIL_UTIL_BASE64_H_

#include <nn/Result.h>

namespace np { namespace util {
	/*!
		@brief Base64 encodes the input data.
		@note Call this function with pOut set to NULL to get the required size of the output buffer.

		@param[out] pOut The buffer to write the encoded data to.
		@param[in,out] outSize The size of the output buffer. If the buffer is too small, this will be set to the required size.
		@param[in] pIn The data to encode.
		@param[in] inSize The size of the input data.

		@return The result of the operation.
	*/
	nn::Result Base64Encode(char* pOut, size_t* outSize, const void* pIn, size_t inSize);
}}	// namespace np::util

#endif	// NP_UTIL_UTIL_BASE64_H_
