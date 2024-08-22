#ifndef NP_CONFIG_H_
#define NP_CONFIG_H_

#if defined(NN_PLATFORM_CTR)
#	ifdef __cplusplus
#		include <np/config/config_Types.h>
#		include <np/config/config_Api.h>
#		include <np/config/config_TitleList.h>
#	endif
#else
#	error no platform selected
#endif	// NN_PLATFORM_CTR

#endif	// NP_CONFIG_H_
