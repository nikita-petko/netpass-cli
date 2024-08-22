#ifndef NN_AM_AM_APISYSTEMMENU_H_
#define NN_AM_AM_APISYSTEMMENU_H_

#include <nn/Result.h>

namespace nn { namespace am {
	Result InitializeForSystemMenu();
	Result FinalizeForSystemMenu();

	Result GetDeviceId(bit32* pOut);
}}	// namespace nn::am

#endif	// ifndef NN_AM_AM_APISYSTEMMENU_H_
