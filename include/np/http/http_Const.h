#ifndef NP_HTTP_HTTP_CONST_H_
#define NP_HTTP_HTTP_CONST_H_

#include <nn/types.h>

namespace np { namespace http {

	/*!
		@brief  The alignment of the HTTP buffer.
	*/
	const size_t HTTP_BUFFER_ALIGNMENT = 0x1000;

	/*!
		@brief  The size of the HTTP buffer.
	*/
	const size_t HTTP_POST_BUFFER_SIZE = 0x19000;

}}	// namespace np::http

#endif	// NP_HTTP_HTTP_CONST_H_
