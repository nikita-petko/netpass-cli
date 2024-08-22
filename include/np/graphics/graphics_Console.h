#ifndef NP_GRAPHICS_GRAPHICS_CONSOLE_H_
#define NP_GRAPHICS_GRAPHICS_CONSOLE_H_

#include <gles2/gl2.h>

/*!
	@brief Prints a message to the screen.

	@param[in] display The display to print to.
	@param[in] text    The text to print.
*/
#define PRINTF(display, text, ...) (void)np::graphics::Printf(display, text, ##__VA_ARGS__)

/*!
	@brief Prints a message to the top screen.

	@param[in] text The text to print.
*/
#define PRINTF_DISPLAY0(text, ...) PRINTF(NN_GX_DISPLAY0, text, ##__VA_ARGS__)

/*!
	@brief Prints a message to the bottom screen.

	@param[in] text The text to print.
*/
#define PRINTF_DISPLAY1(text, ...) PRINTF(NN_GX_DISPLAY1, text, ##__VA_ARGS__)

/*!
	@brief Clears the screen.

	@param[in] display The display to clear.
*/
#define CLEAR(display) (void)np::graphics::ClearScreen(display)

/*!
	@brief Clears the top screen.
*/
#define CLEAR_DISPLAY0() CLEAR(NN_GX_DISPLAY0)

/*!
	@brief Clears the bottom screen.
*/
#define CLEAR_DISPLAY1() CLEAR(NN_GX_DISPLAY1)

namespace np { namespace graphics {

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

#endif	// NP_GRAPHICS_GRAPHICS_CONSOLE_H_
