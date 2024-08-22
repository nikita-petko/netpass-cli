#ifndef NP_HTTP_H_
#define NP_HTTP_H_

#if defined(NN_PLATFORM_CTR)
#	ifdef __cplusplus
#		include <np/http/http_Types.h>
#		include <np/http/http_Const.h>
#		include <np/http/http_Result.h>
#		include <np/http/http_Api.h>
#	endif
#else
#	error no platform selected
#endif	// NN_PLATFORM_CTR

#endif	// NP_HTTP_H_
