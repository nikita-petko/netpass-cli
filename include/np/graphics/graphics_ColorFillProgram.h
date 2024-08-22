#ifndef NP_GRAPHICS_GRAPHICS_COLOR_FILL_PROGRAM_OBJECT_H_
#define NP_GRAPHICS_GRAPHICS_COLOR_FILL_PROGRAM_OBJECT_H_

#include <np/graphics/graphics_ProgramObject.h>

namespace np { namespace graphics {
	/*!
		@brief Represents a shader program for drawing polygons with the vertex color.
	*/
	class ColorFillProgram : public ProgramObject
	{
	public:
		/*!
			@brief Constructor.
		*/
		ColorFillProgram(void);

		/*!
			@brief Destructor.
		*/
		virtual ~ColorFillProgram(void);

	public:
		/*!
			@brief Initializes the shader program object.
		*/
		virtual bool Initialize(const GLuint shaderId);

		/*!
			@brief Finalizes the shader program object.
		*/
		virtual bool Finalize(void);

	public:
		/*!
			@brief Uses the shader program object.
		*/
		virtual bool Use(void);

	protected:
		virtual void InitializeVertexAttributes(void);
	};

}}	// namespace np::graphics

#endif	// NN_NP_GRAPHICS_GRAPHICS_CTR_GRAPHICS_COLOR_FILL_PROGRAM_OBJECT_H_
