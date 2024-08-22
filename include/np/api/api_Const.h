#ifndef NP_API_API_CONST_H_
#define NP_API_API_CONST_H_

#include <nn/cec.h>

namespace np { namespace api {

	/*!
		@brief The maximum size of a message in bytes.
	*/
	static const size_t MESSAGE_MAX_SIZE = sizeof(nn::cec::CecMessageHeader) + nn::cec::Message::MESSAGE_BODY_SIZE_MAX;

}}	// namespace np::api

#endif	// NP_API_API_CONST_H_
