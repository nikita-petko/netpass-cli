#ifndef NP_HTTP_HTTP_TYPES_H_
#define NP_HTTP_HTTP_TYPES_H_

#include <nn/types.h>

namespace np { namespace http {

	/*!
		@brief  A struct detailing a header.
	*/
	struct HttpHeader
	{
		const char8* key;	// The key of the header.
		const char8* value;	// The value of the header.
	};

}}	// namespace np::http

#endif	// NP_HTTP_HTTP_TYPES_H_
