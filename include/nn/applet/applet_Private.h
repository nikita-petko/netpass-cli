#ifndef NN_APPLET_APPLET_PRIVATE_H_
#define NN_APPLET_APPLET_PRIVATE_H_

#include <nn/applet.h>

namespace nn { namespace applet { namespace CTR { namespace detail {
	void CloseApplicationCore(const u8* pParam = NULL, size_t paramSize = 0, Handle handle = NN_APPLET_HANDLE_NONE);
}}}}  // namespace nn::applet::CTR::detail

#endif	// NN_APPLET_APPLET_PRIVATE_H_
