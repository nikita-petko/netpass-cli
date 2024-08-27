#ifndef NP_API_H_
#define NP_API_H_

#if defined(NN_PLATFORM_CTR)
#	ifdef __cplusplus
#		include <np/api/api_Result.h>
#		include <np/api/api_Location.h>
#		include <np/api/api_Boxes.h>
#		include <np/api/api_Passes.h>
#	endif
#else
#	error no platform selected
#endif	// NN_PLATFORM_CTR

#endif	// NP_API_H_
