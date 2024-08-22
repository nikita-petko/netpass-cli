#include <np/graphics/graphics_ProgramObject.h>

namespace np { namespace graphics {

	/* ------------------------------------------------------------------------
			ProgramObject Class Member Function
	   ------------------------------------------------------------------------ */

	ProgramObject::ProgramObject(void)
	: m_Initialized(false)
	, m_VertexAttributes(VERTEX_NONE_ATTRIBUTE)
	, m_Use3d(false)
	, m_ProgramId(0)
	, m_ProjectionMatrix()
	, m_ModelViewMatrix()
	{
		for (u32 shaderIndex = 0; shaderIndex < PROGRAM_OBJECT_MAX_SHADERS; shaderIndex++)
		{
			m_ShaderIds[shaderIndex] = 0;
		}

		for (u32 locationIndex = 0; locationIndex < MAX_UNIFORM_LOCATIONS_NUM; locationIndex++)
		{
			m_UniformLocation[locationIndex] = -1;
		}

		MTX44Identity(&m_ProjectionMatrix);
		MTX44Identity(&m_ModelViewMatrix);
	}

	ProgramObject::~ProgramObject(void) {}

	bool ProgramObject::Begin(void) { return true; }

	bool ProgramObject::End(void) { return true; }

	u32 ProgramObject::GetVertexAttributes(void) { return m_VertexAttributes; }

	void ProgramObject::SetUse3d(const bool use3d) { m_Use3d = use3d; }

	void ProgramObject::SetProjectionMatrix(const nn::math::MTX44& projectionMatrix) { m_ProjectionMatrix = projectionMatrix; }

	void ProgramObject::SetModelViewMatrix(const nn::math::MTX44& modelViewMatrix) { m_ModelViewMatrix = modelViewMatrix; }

	void ProgramObject::SetTextureId(const GLuint textureId) { (void)textureId; }

	void ProgramObject::Update(void) {}

	void ProgramObject::InitializeVertexAttributes(void) { m_VertexAttributes = VERTEX_NONE_ATTRIBUTE; }

	void ProgramObject::InitializeUniforms(void)
	{
		if (m_ProgramId == 0)
		{
			NN_TPANIC_("m_ProgramId is 0.\n");
			return;
		}

		// Vertex shader uniform.
		m_UniformLocation[VERTEX_UNIFORM_PROJECTION] = glGetUniformLocation(m_ProgramId, "uProjection");
		m_UniformLocation[VERTEX_UNIFORM_MODELVIEW]	 = glGetUniformLocation(m_ProgramId, "uModelView");

		// uniform for the reserved fragment shader alpha test.
		m_UniformLocation[FRAGMENT_UNIFORM_ALPHA_TEST] = glGetUniformLocation(m_ProgramId, "dmp_FragOperation.enableAlphaTest");
		// uniform for the reserved fragment shader texture sampler.
		m_UniformLocation[FRAGMENT_UNIFORM_TEXTURE0_SAMPLER_TYPE] = glGetUniformLocation(m_ProgramId, "dmp_Texture[0].samplerType");
		m_UniformLocation[FRAGMENT_UNIFORM_TEXTURE1_SAMPLER_TYPE] = glGetUniformLocation(m_ProgramId, "dmp_Texture[1].samplerType");
		m_UniformLocation[FRAGMENT_UNIFORM_TEXTURE2_SAMPLER_TYPE] = glGetUniformLocation(m_ProgramId, "dmp_Texture[2].samplerType");
		m_UniformLocation[FRAGMENT_UNIFORM_TEXTURE3_SAMPLER_TYPE] = glGetUniformLocation(m_ProgramId, "dmp_Texture[3].samplerType");

		// uniform for the reserved fragment shader texture combiner.
		m_UniformLocation[FRAGMENT_UNIFORM_TEXENV2_SRC_RGB]		  = glGetUniformLocation(m_ProgramId, "dmp_TexEnv[2].srcRgb");
		m_UniformLocation[FRAGMENT_UNIFORM_TEXENV2_SRC_ALPHA]	  = glGetUniformLocation(m_ProgramId, "dmp_TexEnv[2].srcAlpha");
		m_UniformLocation[FRAGMENT_UNIFORM_TEXENV2_OPERAND_RGB]	  = glGetUniformLocation(m_ProgramId, "dmp_TexEnv[2].operandRgb");
		m_UniformLocation[FRAGMENT_UNIFORM_TEXENV2_OPERAND_ALPHA] = glGetUniformLocation(m_ProgramId, "dmp_TexEnv[2].operandAlpha");
		m_UniformLocation[FRAGMENT_UNIFORM_TEXENV2_COMBINE_RGB]	  = glGetUniformLocation(m_ProgramId, "dmp_TexEnv[2].combineRgb");
		m_UniformLocation[FRAGMENT_UNIFORM_TEXENV2_COMBINE_ALPHA] = glGetUniformLocation(m_ProgramId, "dmp_TexEnv[2].combineAlpha");
		m_UniformLocation[FRAGMENT_UNIFORM_TEXENV2_CONST_RGBA]	  = glGetUniformLocation(m_ProgramId, "dmp_TexEnv[2].constRgba");
	}

	void ProgramObject::UpdateModelViewProjectionMatrix(void)
	{
		if (m_ProgramId == 0)
		{
			NN_TPANIC_("m_ProgramId is 0.\n");
			return;
		}

		if (m_Use3d)
		{
			glUniformMatrix4fv(m_UniformLocation[VERTEX_UNIFORM_PROJECTION], 1, GL_FALSE, static_cast<f32*>(m_ProjectionMatrix));
			glUniformMatrix4fv(m_UniformLocation[VERTEX_UNIFORM_MODELVIEW], 1, GL_FALSE, static_cast<f32*>(m_ModelViewMatrix));
		}
		else
		{
			static bool	   initArrayFlag = false;
			static GLfloat projectionArray[16];
			static GLfloat modelViewArray[16];

			if (!initArrayFlag)
			{
				for (u32 i = 0; i < 4; i++)
				{
					for (u32 j = 0; j < 4; j++)
					{
						if (i == j)
						{
							projectionArray[4 * i + j] = 1.0f;
							modelViewArray[4 * i + j]  = 1.0f;
						}
						else
						{
							projectionArray[4 * i + j] = 0.0f;
							modelViewArray[4 * i + j]  = 0.0f;
						}
					}
				}

				initArrayFlag = true;
			}

			glUniformMatrix4fv(m_UniformLocation[VERTEX_UNIFORM_PROJECTION], 1, GL_FALSE, projectionArray);
			glUniformMatrix4fv(m_UniformLocation[VERTEX_UNIFORM_MODELVIEW], 1, GL_FALSE, modelViewArray);
		}
	}

}}	// namespace np::graphics
