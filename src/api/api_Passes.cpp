#include <np/api.h>
#include <np/http.h>

#include <np/log.h>

#include "api/api_Urls.h"

namespace np { namespace api {

	nn::Result RequestPass(nn::cec::TitleId titleId)
	{
		NN_LOG_INFO("Requesting pass for title %d", titleId);

		nn::Result result;

		char8 url[sizeof(API_URL_REQUEST_PASSES) + 16];
		nn::nstd::TSNPrintf(url, sizeof(url), API_URL_REQUEST_PASSES, titleId);

		s32 statusCode = 0;
		result		   = np::http::Put(url, NULL, 0, &statusCode);

		if (result.IsFailure())
		{
			NN_LOG_ERROR("Failed to request pass:");
			NN_DBG_PRINT_RESULT(result);

			return result;
		}

		if (statusCode == 404)
		{
			NN_LOG_WARN("No suitable pass found");

			return ResultNoSuitablePassFound();
		}

		return result;
	}

}}	// namespace np::api
