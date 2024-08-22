#ifndef NP_GRAPHICS_GRAPHICS_TEXTUTILITY_H_
#define NP_GRAPHICS_GRAPHICS_TEXTUTILITY_H_

#include <np/graphics/graphics_Color.h>
#include <np/graphics/graphics_RenderSystemDrawing.h>

namespace np { namespace graphics {

	/*!
		@brief Write text centered on the screen.
		@attention Only call this from within a render command.

		@param[in] renderSystem The render system to use.
		@param[in] text The text to write.
	*/
	void DrawCenteredText(RenderSystemDrawing* renderSystem, const char* text, ...);

	/*!
		@brief Draw a fatality message on the screen.

		@param[in] text The text to write.
	*/
	void DrawFatality(const char* text);

	/*!
		@brief  Draw a status screen.

		@param[in] color The color to use.
		@param[in] text The text to write.
		@param[in] textColor The color of the text. Default is white.
	*/
	void DrawStatusScreen(const Color color, const char* text, const Color textColor = COLOR_WHITE);

}}	// namespace np::graphics

#endif	// NP_GRAPHICS_GRAPHICS_TEXTUTILITY_H_
