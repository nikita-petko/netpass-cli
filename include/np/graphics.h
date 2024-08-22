#ifndef NP_GRAPHICS_H_
#define NP_GRAPHICS_H_

#if defined(NN_PLATFORM_CTR)
#	ifdef __cplusplus
#		include <np/graphics/graphics_MemoryManager.h>
#		include <np/graphics/graphics_TextureConverter.h>
#		include <np/graphics/graphics_Utility.h>

#		include <np/graphics/graphics_Light.h>
#		include <np/graphics/graphics_RenderSystem.h>
#		include <np/graphics/graphics_RenderSystemDrawing.h>
#		include <np/graphics/graphics_RenderSystemExt.h>

#		include <np/graphics/graphics_CommandCache.h>

// Imports defined by us.
#		include <np/graphics/graphics_Api.h>
#		include <np/graphics/graphics_Font.h>
#		include <np/graphics/graphics_Color.h>
#		include <np/graphics/graphics_TextUtility.h>
#		include <np/graphics/graphics_Console.h>
#	endif	// __cplusplus
#else
#	error no platform selected
#endif	// NN_PLATFORM_CTR

#endif	// NP_GRAPHICS_H_
