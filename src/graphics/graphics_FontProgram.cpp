#include <np/graphics/graphics_FontProgram.h>

namespace np { namespace graphics {

	extern u64 graphicsFontBitmap[];

	/* ------------------------------------------------------------------------
			FontProgram Class Member Function
	   ------------------------------------------------------------------------ */

	FontProgram::FontProgram(void) : ProgramObject(), m_FontTextureId(0) { InitializeVertexAttributes(); }

	FontProgram::~FontProgram(void) { Finalize(); }

	bool FontProgram::Initialize(const GLuint shaderId)
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
		glBindAttribLocation(m_ProgramId, 2, "aTexCoord");

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

		GRAPHICS_ASSERT_GL_ERROR();

		InitializeFontTexture();
		GRAPHICS_ASSERT_GL_ERROR();

		ProgramObject::InitializeUniforms();
		GRAPHICS_ASSERT_GL_ERROR();

		m_Initialized = true;

		return true;
	}

	bool FontProgram::Finalize(void)
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

		if (m_FontTextureId != 0)
		{
			glDeleteTextures(1, &m_FontTextureId);
			m_FontTextureId = 0;
		}

		m_Initialized = false;

		return true;
	}

	bool FontProgram::Begin(void)
	{
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glBlendEquation(GL_FUNC_ADD);

		glDisable(GL_CULL_FACE);
		glDisable(GL_COLOR_LOGIC_OP);

		return true;
	}

	bool FontProgram::Use(void)
	{
		if (!m_Initialized)
		{
			return false;
		}

		static bool	   initArrayFlag = false;
		static GLfloat fontColor[4];
		if (!initArrayFlag)
		{
			fontColor[0] = 1.0f;
			fontColor[1] = 1.0f;
			fontColor[2] = 1.0f;
			fontColor[3] = 1.0f;

			initArrayFlag = true;
		}

		glUseProgram(m_ProgramId);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, m_FontTextureId);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

		UpdateModelViewProjectionMatrix();

		// uniform for the reserved fragment shader alpha test.
		glUniform1i(m_UniformLocation[FRAGMENT_UNIFORM_ALPHA_TEST], GL_FALSE);

		// uniform for the reserved fragment shader texture sampler.
		glUniform1i(m_UniformLocation[FRAGMENT_UNIFORM_TEXTURE0_SAMPLER_TYPE], GL_TEXTURE_2D);
		glUniform1i(m_UniformLocation[FRAGMENT_UNIFORM_TEXTURE1_SAMPLER_TYPE], GL_FALSE);
		glUniform1i(m_UniformLocation[FRAGMENT_UNIFORM_TEXTURE2_SAMPLER_TYPE], GL_FALSE);
		glUniform1i(m_UniformLocation[FRAGMENT_UNIFORM_TEXTURE3_SAMPLER_TYPE], GL_FALSE);

		// uniform for the reserved fragment shader texture combiner.
		glUniform3i(m_UniformLocation[FRAGMENT_UNIFORM_TEXENV2_SRC_RGB], GL_TEXTURE0, GL_PRIMARY_COLOR, GL_CONSTANT);
		glUniform3i(m_UniformLocation[FRAGMENT_UNIFORM_TEXENV2_SRC_ALPHA], GL_TEXTURE0, GL_PRIMARY_COLOR, GL_CONSTANT);
		glUniform3i(m_UniformLocation[FRAGMENT_UNIFORM_TEXENV2_OPERAND_RGB], GL_SRC_COLOR, GL_SRC_COLOR, GL_SRC_COLOR);
		glUniform3i(m_UniformLocation[FRAGMENT_UNIFORM_TEXENV2_OPERAND_ALPHA], GL_SRC_ALPHA, GL_SRC_ALPHA, GL_SRC_ALPHA);
		glUniform1i(m_UniformLocation[FRAGMENT_UNIFORM_TEXENV2_COMBINE_RGB], GL_MODULATE);
		glUniform1i(m_UniformLocation[FRAGMENT_UNIFORM_TEXENV2_COMBINE_ALPHA], GL_MODULATE);
		glUniform4fv(m_UniformLocation[FRAGMENT_UNIFORM_TEXENV2_CONST_RGBA], 1, fontColor);

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glBlendEquation(GL_FUNC_ADD);

		return true;
	}

	bool FontProgram::End(void)
	{
		glDisable(GL_BLEND);

		return true;
	}

	void FontProgram::InitializeVertexAttributes(void)
	{
		m_VertexAttributes = VERTEX_POSITION_ATTRIBUTE | VERTEX_COLOR_ATTRIBUTE | VERTEX_TEXCOORD_ATTRIBUTE;
	}

	void FontProgram::InitializeFontTexture(void)
	{
		static u8 fontBitmap[64 * 128 * 2];

		glGenTextures(1, &m_FontTextureId);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, m_FontTextureId);

		u32 offset	  = 0;
		u32 offsetDst = 0;

		for (u32 i = 0; i < 128; i++)
		{
			for (u32 j = 0; j < 8; j++)
			{
				u64 pixels = (*(graphicsFontBitmap + offset));

				for (u32 k = 0; k < 8; k++)
				{
					(*(fontBitmap + offsetDst))		= static_cast<u8>(pixels >> (64 - (8 - k) * 8));
					(*(fontBitmap + offsetDst + 1)) = static_cast<u8>(pixels >> (64 - (8 - k) * 8));
					offsetDst += 2;
				}

				offset++;
			}
		}

		glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE_ALPHA, 64, 128, 0, GL_LUMINANCE_ALPHA, GL_UNSIGNED_BYTE, fontBitmap);
	}

}}	// namespace np::graphics
