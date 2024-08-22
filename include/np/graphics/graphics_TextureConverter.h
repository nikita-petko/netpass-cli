#ifndef NP_GRAPHICS_GRAPHICS_TEXTURE_CONVERTER_H_
#define NP_GRAPHICS_GRAPHICS_TEXTURE_CONVERTER_H_

#include <nn/config.h>
#include <nn/gx.h>

#include <gles2/gl2ext.h>

namespace np { namespace graphics {

	/* Please see man pages for details */
	bool ConvertGLTextureToNative(const GLenum format, const u32 width, const u32 height, void* glData, void* dmpData);

}}	// namespace np::graphics

#endif
