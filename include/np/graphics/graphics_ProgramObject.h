#ifndef NP_GRAPHICS_GRAPHICS_PROGRAM_OBJECT_H_
#define NP_GRAPHICS_GRAPHICS_PROGRAM_OBJECT_H_

#include <nn/gx.h>
#include <nn/math.h>

#include <np/graphics/graphics_TrianglesRenderData.h>
#include <np/graphics/graphics_Utility.h>

namespace np { namespace graphics {
	/*!
		@brief Sets the maximum number of shaders that the ProgramObject class can hold.
	*/
	const u32 PROGRAM_OBJECT_MAX_SHADERS = 1;

	/*!
		@brief Represents the base class for shader programs.
	*/
	class ProgramObject : private nn::util::NonCopyable<ProgramObject>
	{
	public:
		/*!
			@brief Constructor.
		*/
		ProgramObject(void);

		/*!
			@brief Destructor.
		*/
		virtual ~ProgramObject(void);

	public:
		/*!
			@brief Initializes the shader program object.
		*/
		virtual bool Initialize(const GLuint shaderId) = 0;

		/*!
			@brief Finalizes the shader program object.
		*/
		virtual bool Finalize(void) = 0;

	public:
		/*!
			@brief Sets the rendering state before using the shader program object.
		*/
		virtual bool Begin(void);

		/*!
			@brief Uses the shader program object.
		*/
		virtual bool Use(void) = 0;

		/*!
			@brief Sets the rendering state after using the shader program object.
		*/
		virtual bool End(void);

	public:
		/*!
			@brief Gets vertex attributes set in the shader program object.

			@return Returns vertex attributes.
		*/
		virtual u32 GetVertexAttributes(void);

		/*!
			@brief Sets whether to convert the three-dimensional vertex position coordinates to clip coordinates using the projection matrix
		   and model view matrix.

			@param[in] use3d Specifies whether to convert to the clip coordinate system (true or false).
		*/
		virtual void SetUse3d(const bool use3d);

		/*!
			@brief Sets the projection matrix for the shader program object.

			@param[in] projectionMatrix Specifies the projection matrix.
		*/
		virtual void SetProjectionMatrix(const nn::math::MTX44& projectionMatrix);

		/*!
			@brief Sets the model view matrix for the shader program object.

			@param[in] modelViewMatrix Specifies the model view matrix.
		*/
		virtual void SetModelViewMatrix(const nn::math::MTX44& modelViewMatrix);

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
		virtual void InitializeUniforms(void);
		virtual void UpdateModelViewProjectionMatrix(void);

	protected:
		bool m_Initialized;
		bool m_Padding0[3];

		u32	 m_VertexAttributes;
		bool m_Use3d;
		u8	 m_Padding[3];

		GLuint m_ProgramId;
		GLuint m_ShaderIds[PROGRAM_OBJECT_MAX_SHADERS];
		GLint  m_UniformLocation[MAX_UNIFORM_LOCATIONS_NUM];

		nn::math::MTX44 m_ProjectionMatrix;
		nn::math::MTX44 m_ModelViewMatrix;
	};
}}	// namespace np::graphics

#endif
