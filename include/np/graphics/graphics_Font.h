#ifndef NP_GRAPHICS_GRAPHICS_FONT_H_
#define NP_GRAPHICS_GRAPHICS_FONT_H_

/*!
	@brief The font size to use for rendering text.
*/
#define FONT_SIZE 8

/*!
	@brief Converts a character count to a pixel count.

	@param[in] x The character count.
*/
#define CHARS(x) ((x - 1) * FONT_SIZE)	// Subtract 1 to account for the space between characters

/*!
	@brief Converts a line count to a pixel count.

	@param[in] x The line count.
*/
#define LINE(x) ((x) * FONT_SIZE)

#endif	// NP_GRAPHICS_GRAPHICS_FONT_H_
