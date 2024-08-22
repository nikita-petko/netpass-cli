#include <np/graphics/graphics_ColorFillProgram.h>

namespace np { namespace graphics {

	/* ------------------------------------------------------------------------
			ColorFillProgram Class Member Function
	   ------------------------------------------------------------------------ */

	ColorFillProgram::ColorFillProgram(void) : ProgramObject() { InitializeVertexAttributes(); }

	ColorFillProgram::~ColorFillProgram(void) { Finalize(); }

	bool ColorFillProgram::Initialize(const GLuint shaderId)
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
		glBindAttribLocation(m_ProgramId, 1, "aColor");

		// Link.
		glLinkProgram(m_ProgramId);

		// Validate.
		GLint param = 0;
		glValidateProgram(m_ProgramId);
		glGetProgramiv(m_ProgramId, GL_VALIDATE_STATUS, &param);
		if (param != GL_TRUE)
		{
			NN_TPANIC_("GL_VALIDATE_STATUS returned GL_FALSE.\n");

			return false;
		}

		ProgramObject::InitializeUniforms();
		GRAPHICS_ASSERT_GL_ERROR();

		m_Initialized = true;

		return true;
	}

	bool ColorFillProgram::Finalize(void)
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

	bool ColorFillProgram::Use(void)
	{
		if (!m_Initialized)
		{
			return false;
		}

		glUseProgram(m_ProgramId);
		glBindTexture(GL_TEXTURE_2D, 0);

		UpdateModelViewProjectionMatrix();

		// uniform for the reserved fragment shader alpha test.
		glUniform1i(m_UniformLocation[FRAGMENT_UNIFORM_ALPHA_TEST], GL_FALSE);

		// uniform for the reserved fragment shader texture sampler.
		glUniform1i(m_UniformLocation[FRAGMENT_UNIFORM_TEXTURE0_SAMPLER_TYPE], GL_FALSE);
		glUniform1i(m_UniformLocation[FRAGMENT_UNIFORM_TEXTURE1_SAMPLER_TYPE], GL_FALSE);
		glUniform1i(m_UniformLocation[FRAGMENT_UNIFORM_TEXTURE2_SAMPLER_TYPE], GL_FALSE);
		glUniform1i(m_UniformLocation[FRAGMENT_UNIFORM_TEXTURE3_SAMPLER_TYPE], GL_FALSE);

		// uniform for the reserved fragment shader texture combiner.
		glUniform3i(m_UniformLocation[FRAGMENT_UNIFORM_TEXENV2_SRC_RGB], GL_PRIMARY_COLOR, GL_PREVIOUS, GL_PREVIOUS);
		glUniform3i(m_UniformLocation[FRAGMENT_UNIFORM_TEXENV2_SRC_ALPHA], GL_PRIMARY_COLOR, GL_PREVIOUS, GL_PREVIOUS);
		glUniform3i(m_UniformLocation[FRAGMENT_UNIFORM_TEXENV2_OPERAND_RGB], GL_SRC_COLOR, GL_SRC_COLOR, GL_SRC_COLOR);
		glUniform3i(m_UniformLocation[FRAGMENT_UNIFORM_TEXENV2_OPERAND_ALPHA], GL_SRC_ALPHA, GL_SRC_ALPHA, GL_SRC_ALPHA);
		glUniform1i(m_UniformLocation[FRAGMENT_UNIFORM_TEXENV2_COMBINE_RGB], GL_REPLACE);
		glUniform1i(m_UniformLocation[FRAGMENT_UNIFORM_TEXENV2_COMBINE_ALPHA], GL_REPLACE);

		return true;
	}

	void ColorFillProgram::InitializeVertexAttributes(void) { m_VertexAttributes = VERTEX_POSITION_ATTRIBUTE | VERTEX_COLOR_ATTRIBUTE; }

}}	// namespace np::graphics
