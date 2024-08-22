#include <np/api.h>
#include <np/http.h>
#include <np/config.h>
#include <np/graphics.h>

#include <np/log.h>

#include "api/api_Urls.h"

namespace {
s8 s_Location = -1;
}  // namespace

namespace np { namespace api {

	s8 GetLocalLocation(void)
	{
		return s_Location;
	}

	void SetLocalLocation(s8 location)
	{
		s_Location = location;
	}

	nn::Result GetLocation(s8* location)
	{
		NN_LOG_INFO("Getting location from API");

		nn::Result result;

		s32 statusCode = 0;

		s8 response;
		result = np::http::Get(API_URL_GET_LOCATION, reinterpret_cast<u8*>(&response), sizeof(response), &statusCode);

		*location = response;

		NN_LOG_INFO("Got location: %d, statusCode: %d", *location, statusCode);

		if (result == nn::http::ResultBodyBufShortage())
			// This will always be the case.
			result = nn::ResultSuccess();

		if (statusCode != 200)
		{
			NN_LOG_WARN("Status code was 204, or an error, setting location to -1 to be safe");

			*location = -1;
		}

		return result;
	}

	nn::Result SetLocation(s8 location)
	{
		NN_LOG_INFO("Setting location to %d", location);

		nn::Result result;

		s32 upcastedLocation = static_cast<s32>(location);

		char url[128];
		nn::nstd::TSNPrintf(url, sizeof(url), API_URL_SET_LOCATION, upcastedLocation);

		s32 statusCode = 0;
		result		   = np::http::Put(url, NULL, 0, &statusCode);
		// This API will 409 if the outboxes were not uploaded for the mac address

		if (statusCode == 409)
		{
			NN_LOG_WARN("HTTP request failed with status code %d, did you upload the outboxes?", statusCode);

			return ResultMessageBoxListNotUploaded();
		}

		np::config::GetConfiguration()->last_location = location;
		np::config::FlushConfiguration();

		PRINTF_DISPLAY1("Location set to %d", location);

		return result;
	}

}}	// namespace np::api
