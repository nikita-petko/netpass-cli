#ifndef NP_GRAPHICS_GRAPHICS_COLOR_H_
#define NP_GRAPHICS_GRAPHICS_COLOR_H_

#include <nn/types.h>

/*!
	@brief Represents the color white.
*/
#define GRAPHICS_COLOR_WHITE 1.0f, 1.0f, 1.0f, 1.0f

/*!
	@brief Represents the color black.
*/
#define GRAPHICS_COLOR_BLACK 0.0f, 0.0f, 0.0f, 1.0f

/*!
	@brief Represents the color red.
*/
#define GRAPHICS_COLOR_RED 1.0f, 0.0f, 0.0f, 1.0f

/*!
	@brief Represents the color green.
*/
#define GRAPHICS_COLOR_GREEN 0.0f, 1.0f, 0.0f, 1.0f

/*!
	@brief Represents the color blue.
*/
#define GRAPHICS_COLOR_BLUE 0.0f, 0.0f, 1.0f, 1.0f

/*!
	@brief Represents the color orange.
*/
#define GRAPHICS_COLOR_ORANGE 1.0f, 0.5f, 0.0f, 1.0f

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

}}	// namespace np::graphics

#endif	// NP_GRAPHICS_GRAPHICS_COLOR_H_
