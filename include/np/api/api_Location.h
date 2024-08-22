#ifndef NP_API_API_LOCATION_H_
#define NP_API_API_LOCATION_H_

#include <nn/Result.h>

namespace np { namespace api {

    /*!
        @brief Gets the current stored location.

        @return The location.
    */
    s8 GetLocalLocation();

    /*!
        @brief Sets the currently stored location.

        @param[in] location The location to set.
    */
    void SetLocalLocation(s8 location);

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

}}	// namespace np::api

#endif	// NP_API_API_LOCATION_H_
