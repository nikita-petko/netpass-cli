#ifndef NP_UTIL_H_
#define NP_UTIL_H_

#if defined(NN_PLATFORM_CTR)
#	ifdef __cplusplus
#		include <np/util/util_Base64.h>
#		include <np/util/util_Cec.h>
#	endif
#else
#	error no platform selected
#endif	// NN_PLATFORM_CTR

#endif	// NP_UTIL_H_
