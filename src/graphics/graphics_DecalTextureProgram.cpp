#include <np/graphics/graphics_DecalTextureProgram.h>

namespace np { namespace graphics {

	/* ------------------------------------------------------------------------
			DecalTextureProgram Class Member Function
	   ------------------------------------------------------------------------ */

	DecalTextureProgram::DecalTextureProgram(void) : ProgramObject(), m_TextureId(0) { InitializeVertexAttributes(); }

	DecalTextureProgram::~DecalTextureProgram(void) { Finalize(); }

	bool DecalTextureProgram::Initialize(const GLuint shaderId)
	{
		if (m_Initialized)
		{
			return false;
		}

		m_ProgramId = glCreateProgram();

		// Vertex shader.
		m_ShaderIds[0] = shaderId;
		glAttachShader(m_ProgramId, m_ShaderIds[0]);

		// Fragment shader.
		glAttachShader(m_ProgramId, GL_DMP_FRAGMENT_SHADER_DMP);

		// Bind.
		glBindAttribLocation(m_ProgramId, 0, "aPosition");
		glBindAttribLocation(m_ProgramId, 1, "aTexCoord");

		// Link.
		glLinkProgram(m_ProgramId);

		// Validate.
		GLint param = 0;
		glValidateProgram(m_ProgramId);
		glGetProgramiv(m_ProgramId, GL_VALIDATE_STATUS, &param);
		if (param != GL_TRUE)
		{
			NN_TPANIC_("[GRAPHICS] : GL_VALIDATE_STATUS returned GL_FALSE.\n");

			return false;
		}

		ProgramObject::InitializeUniforms();

		GRAPHICS_ASSERT_GL_ERROR();

		m_Initialized = true;

		return true;
	}

	bool DecalTextureProgram::Finalize(void)
	{
		if (!m_Initialized)
		{
			return false;
		}

		if (m_ProgramId != 0)
		{
			glDeleteProgram(m_ProgramId);
			m_ProgramId = 0;
		}

		// Vertex shader.
		if (m_ShaderIds[0] != 0)
		{
			glDeleteShader(m_ShaderIds[0]);
			m_ShaderIds[0] = 0;
		}

		m_Initialized = false;

		return true;
	}

	bool DecalTextureProgram::Use(void)
	{
		if (!m_Initialized)
		{
			return false;
		}

		glUseProgram(m_ProgramId);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, m_TextureId);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

		UpdateModelViewProjectionMatrix();

		// uniform for the reserved fragment shader alpha test.
		glUniform1i(m_UniformLocation[FRAGMENT_UNIFORM_ALPHA_TEST], GL_FALSE);

		// uniform for the reserved fragment shader texture sampler.
		glUniform1i(m_UniformLocation[FRAGMENT_UNIFORM_TEXTURE0_SAMPLER_TYPE], GL_TEXTURE_2D);
		glUniform1i(m_UniformLocation[FRAGMENT_UNIFORM_TEXTURE1_SAMPLER_TYPE], GL_FALSE);
		glUniform1i(m_UniformLocation[FRAGMENT_UNIFORM_TEXTURE2_SAMPLER_TYPE], GL_FALSE);
		glUniform1i(m_UniformLocation[FRAGMENT_UNIFORM_TEXTURE3_SAMPLER_TYPE], GL_FALSE);

		// uniform for the reserved fragment shader texture combiner.
		glUniform3i(m_UniformLocation[FRAGMENT_UNIFORM_TEXENV2_SRC_RGB], GL_TEXTURE0, GL_CONSTANT, GL_CONSTANT);
		glUniform3i(m_UniformLocation[FRAGMENT_UNIFORM_TEXENV2_SRC_ALPHA], GL_TEXTURE0, GL_CONSTANT, GL_CONSTANT);
		glUniform3i(m_UniformLocation[FRAGMENT_UNIFORM_TEXENV2_OPERAND_RGB], GL_SRC_COLOR, GL_SRC_COLOR, GL_SRC_COLOR);
		glUniform3i(m_UniformLocation[FRAGMENT_UNIFORM_TEXENV2_OPERAND_ALPHA], GL_SRC_ALPHA, GL_SRC_ALPHA, GL_SRC_ALPHA);
		glUniform1i(m_UniformLocation[FRAGMENT_UNIFORM_TEXENV2_COMBINE_RGB], GL_REPLACE);
		glUniform1i(m_UniformLocation[FRAGMENT_UNIFORM_TEXENV2_COMBINE_ALPHA], GL_REPLACE);

		return true;
	}

	void DecalTextureProgram::SetTextureId(const GLuint textureId) { m_TextureId = textureId; }

	void DecalTextureProgram::Update(void) { glBindTexture(GL_TEXTURE_2D, m_TextureId); }

	void DecalTextureProgram::InitializeVertexAttributes(void)
	{
		m_VertexAttributes = VERTEX_POSITION_ATTRIBUTE | VERTEX_TEXCOORD_ATTRIBUTE;
	}

}}	// namespace np::graphics
