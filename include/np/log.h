#ifndef NP_LOG_H_
#define NP_LOG_H_

#if defined(NN_PLATFORM_CTR)
#	ifdef NP_LOGGING_ENABLED
#		ifdef __cplusplus
#			include <np/log/log_Api.h>
#			include <np/log/log_Macros.h>
#		endif
#	endif	// NP_LOGGING_ENABLED
#else
#	error no platform selected
#endif	// NN_PLATFORM_CTR

#endif	// NP_LOG_H_
