#ifndef NN_NWM_H_
#define NN_NWM_H_

#if defined(NN_PLATFORM_CTR)
#	include <nn/nwm/CTR/nwm_API.h>
#	include <nn/nwm/nwm_Mac.h>

namespace nn { namespace nwm {
	using namespace nn::nwm::CTR;
}}	// namespace nn::nwm
#else
#	error no platform selected
#endif

#endif	// NN_NWM_H_
