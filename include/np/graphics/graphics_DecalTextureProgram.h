#ifndef NP_GRAPHICS_GRAPHICS_DECAL_TEXTURE_PROGRAM_OBJECT_H_
#define NP_GRAPHICS_GRAPHICS_DECAL_TEXTURE_PROGRAM_OBJECT_H_

#include <np/graphics/graphics_ProgramObject.h>

namespace np { namespace graphics {
	/*!
		@brief Represents a shader program for placing texture decals on polygons.
	*/
	class DecalTextureProgram : public ProgramObject
	{
	public:
		/*!
			@brief Constructor.
		*/
		DecalTextureProgram(void);

		/*!
			@brief Destructor.
		*/
		virtual ~DecalTextureProgram(void);

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

	public:
		/*!
			@brief Sets the handle of a texture object.

			@param[in] textureId Specifies the texture object handle.
		*/
		virtual void SetTextureId(const GLuint textureId);

		/*!
			@brief Updates the state of the shader program object.
		*/
		virtual void Update(void);

	protected:
		virtual void InitializeVertexAttributes(void);

	protected:
		GLuint m_TextureId;
	};

}}	// namespace np::graphics

#endif
