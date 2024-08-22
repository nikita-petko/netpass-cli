#ifndef NP_GRAPHICS_GRAPHICS_FONT_PROGRAM_OBJECT_H_
#define NP_GRAPHICS_GRAPHICS_FONT_PROGRAM_OBJECT_H_

#include <np/graphics/graphics_ProgramObject.h>

namespace np { namespace graphics {

	/*!
		@brief Represents a shader program for rendering fonts.
	*/
	class FontProgram : public ProgramObject
	{
	public:
		/*!
			@brief Constructor.
		*/
		FontProgram(void);

		/*!
			@brief Destructor.
		*/
		virtual ~FontProgram(void);

	public:
		/*!
			@brief Initializes the shader program object.
		*/
		virtual bool Initialize(const GLuint shaderId);

		/*!
			@brief Uses the shader program object.
		*/
		virtual bool Finalize(void);

	public:
		/*!
			@brief Sets the rendering state before using the shader program object.
		*/
		virtual bool Begin(void);

		/*!
			@brief Uses the shader program object.
		*/
		virtual bool Use(void);

		/*!
			@brief Sets the rendering state after using the shader program object.
		*/
		virtual bool End(void);

	protected:
		virtual void InitializeVertexAttributes(void);
		void		 InitializeFontTexture(void);

	protected:
		GLuint m_FontTextureId;
	};
}}	// namespace np::graphics

#endif
