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
#	endif	// __cplusplus

/*!
	@brief The font size to use for rendering text.
*/
#	define FONT_SIZE 8

/*!
	@brief Converts a character count to a pixel count.

	@param[in] x The character count.
*/
#	define CHARS(x) ((x - 1) * FONT_SIZE)	// Subtract 1 to account for the space between characters

/*!
	@brief Converts a line count to a pixel count.

	@param[in] x The line count.
*/
#	define LINE(x) ((x) * FONT_SIZE)

/*!
	@brief Submits a render command to the render queue.

	@param[in] func The function to call.
*/
#	define SUBMIT_RENDER_COMMAND(func) np::graphics::SubmitRenderCommand(func)

/*!
	@brief Submits a render command to the render queue.

	@param[in] func  The function to call.
	@param[in] param The parameter to pass to the function.
*/
#	define SUBMIT_RENDER_COMMAND_PARAM(func, param) np::graphics::SubmitRenderCommand(func, param)

/*!
	@brief Represents the color white.
*/
#	define GRAPHICS_COLOR_WHITE 1.0f, 1.0f, 1.0f, 1.0f

/*!
	@brief Represents the color black.
*/
#	define GRAPHICS_COLOR_BLACK 0.0f, 0.0f, 0.0f, 1.0f

/*!
	@brief Represents the color red.
*/
#	define GRAPHICS_COLOR_RED 1.0f, 0.0f, 0.0f, 1.0f

/*!
	@brief Represents the color green.
*/
#	define GRAPHICS_COLOR_GREEN 0.0f, 1.0f, 0.0f, 1.0f

/*!
	@brief Represents the color blue.
*/
#	define GRAPHICS_COLOR_BLUE 0.0f, 0.0f, 1.0f, 1.0f

/*!
	@brief Represents the color orange.
*/
#	define GRAPHICS_COLOR_ORANGE 1.0f, 0.5f, 0.0f, 1.0f

/*!
	@brief Prints a message to the screen.

	@param[in] display The display to print to.
	@param[in] text    The text to print.
*/
#	define PRINTF(display, text, ...) (void)np::graphics::Printf(display, text, ##__VA_ARGS__)

/*!
	@brief Prints a message to the top screen.

	@param[in] text The text to print.
*/
#	define PRINTF_DISPLAY0(text, ...) PRINTF(NN_GX_DISPLAY0, text, ##__VA_ARGS__)

/*!
	@brief Prints a message to the bottom screen.

	@param[in] text The text to print.
*/
#	define PRINTF_DISPLAY1(text, ...) PRINTF(NN_GX_DISPLAY1, text, ##__VA_ARGS__)

/*!
	@brief Clears the screen.

	@param[in] display The display to clear.
*/
#	define CLEAR(display) (void)np::graphics::ClearScreen(display)

/*!
	@brief Clears the top screen.
*/
#	define CLEAR_DISPLAY0() CLEAR(NN_GX_DISPLAY0)

/*!
	@brief Clears the bottom screen.
*/
#	define CLEAR_DISPLAY1() CLEAR(NN_GX_DISPLAY1)

namespace np { namespace graphics {

	/*!
		@brief Represents a color.

		@note The color is represented as an array of four floating point values.
	*/
	typedef f32 Color[4];

	/*!
		@brief Represents the color white.
	*/
	extern Color COLOR_WHITE;

	/*!
		@brief Represents the color black.
	*/
	extern Color COLOR_BLACK;

	/*!
		@brief Represents the color red.
	*/
	extern Color COLOR_RED;

	/*!
		@brief Represents the color green.
	*/
	extern Color COLOR_GREEN;

	/*!
		@brief Represents the color blue.
	*/
	extern Color COLOR_BLUE;

	/*!
		@brief Represents the color orange.
	*/
	extern Color COLOR_ORANGE;

	/*!
		@brief Command to render, ensures that all rendering commands are done in the same thread.

		@param[in] renderSystem The render system to use.
		@param[in] param The parameter to pass to the function.
	*/
	typedef void (*RenderCommandFunc)(RenderSystemDrawing*, uptr);

	/*!
		@brief Initializes the graphics system.
	*/
	void Initialize(void);

	/*!
		@brief Finalizes the graphics system.
	*/
	void Finalize(void);

	/*!
		@brief Submit a render command to the render queue.

		@param[in] func The function to call.
		@param[in] param The parameter to pass to the function.
	*/
	void SubmitRenderCommand(RenderCommandFunc func, uptr param = NULL);

	/*!
		@brief Submit a render command to the render queue.

		@tparam T The type of the parameter.

		@param[in] func The function to call.
		@param[in] param The parameter to pass to the function.
	*/
	template<typename T>
	void SubmitRenderCommand(void (*func)(RenderSystemDrawing*, T*), T* param)
	{
		SubmitRenderCommand(reinterpret_cast<RenderCommandFunc>(func), reinterpret_cast<uptr>(param));
	}

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

	/*!
		@brief Print a message to the screen.

		@param[in] display The display to print to.
		@param[in] text The text to print.
	*/
	void Printf(GLenum display, const char* text, ...);

	/*!
		@brief Clears the screen and printf state.

		@param[in] display The display to clear.
	*/
	void ClearScreen(GLenum display);
}}	// namespace np::graphics

#else
#	error no platform selected
#endif	// NN_PLATFORM_CTR

#endif	// NP_GRAPHICS_H_
