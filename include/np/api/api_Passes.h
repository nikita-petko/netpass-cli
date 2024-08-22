#ifndef NP_API_API_PASSES_H_
#define NP_API_API_PASSES_H_

#include <nn/cec.h>

namespace np { namespace api {

    /*!
		@brief Requests a pass for the specified title ID be placed into our next inbox.
		@warning This does not guarantee that the pass will be placed into the inbox, only that it will be requested.

		@param[in] titleId The title ID to request a pass for.

		@return The result of the operation.
	*/
	nn::Result RequestPass(nn::cec::TitleId titleId);

}}	// namespace np::api

#endif	// NP_API_API_PASSES_H_
