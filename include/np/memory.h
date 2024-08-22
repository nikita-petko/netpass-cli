#ifndef NP_MEMORY_H_
#define NP_MEMORY_H_

#if defined(NN_PLATFORM_CTR)
#	ifdef __cplusplus
#		include <np/memory/memory_Macros.h>
#		include <np/memory/memory_Api.h>
#	endif
#else
#	error no platform selected
#endif	// NN_PLATFORM_CTR

#endif	// NP_MEMORY_H_
