#include <np/graphics/graphics_ShaderManager.h>

extern "C"
{
	extern u8* BasicVShader_SHADER_BEGIN[];
	extern u8* BasicVShader_SHADER_END[];
}

namespace np { namespace graphics {

	/* ------------------------------------------------------------------------
		ShaderManager Class Member Function
		------------------------------------------------------------------------ */

	ShaderManager::ShaderManager(void) : m_Initialized(false), m_ColorFillProgram(), m_FontProgram(), m_DecalTextureProgram() {}

	ShaderManager::~ShaderManager(void) {}

	bool ShaderManager::Initialize(void)
	{
		if (m_Initialized)
		{
			return false;
		}

		for (u32 shaderBinaryIndex = 0; shaderBinaryIndex < MAX_SHADER_BINARY_NUM; shaderBinaryIndex++)
		{
			for (u32 shaderProgramIndex = 0; shaderProgramIndex < MAX_SHADER_PROGRAM_NUM; shaderProgramIndex++)
			{
				m_ShaderProgramIdArray[shaderBinaryIndex][shaderProgramIndex]  = 0;
				m_ShaderProgramPtrArray[shaderBinaryIndex][shaderProgramIndex] = NULL;
			}
		}

		m_Initialized = true;

		bool flag = InitializeBasicShader();
		if (!flag)
		{
			NN_TPANIC_("Failed to initialize BasicShaderBinary.");
		}

		return true;
	}

	bool ShaderManager::InitializeBasicShader(void)
	{
		for (u32 shaderProgramIndex = 0; shaderProgramIndex < BASIC_SHADER_BINARY_SHADER_NUM; shaderProgramIndex++)
		{
			m_ShaderProgramIdArray[BASIC_SHADER_BINARY_INDEX][shaderProgramIndex] = glCreateShader(GL_VERTEX_SHADER);
		}

		glShaderBinary(BASIC_SHADER_BINARY_SHADER_NUM,
					   &m_ShaderProgramIdArray[BASIC_SHADER_BINARY_INDEX][0],
					   GL_PLATFORM_BINARY_DMP,
					   BasicVShader_SHADER_BEGIN,
					   reinterpret_cast<s32>(BasicVShader_SHADER_END) - reinterpret_cast<s32>(BasicVShader_SHADER_BEGIN));

		u32 shaderBinaryIndex = BASIC_SHADER_BINARY_INDEX;

		// ColorFillShader
		u32	   shaderType = COLOR_FILL_SHADER;
		GLuint shaderId	  = m_ShaderProgramIdArray[BASIC_SHADER_BINARY_INDEX][shaderType];
		m_ColorFillProgram.Initialize(shaderId);
		m_ShaderProgramPtrArray[shaderBinaryIndex][shaderType] = &m_ColorFillProgram;

		// FontShader
		shaderType = FONT_SHADER;
		shaderId   = m_ShaderProgramIdArray[BASIC_SHADER_BINARY_INDEX][shaderType];
		m_FontProgram.Initialize(shaderId);
		m_ShaderProgramPtrArray[shaderBinaryIndex][shaderType] = &m_FontProgram;

		// DecalTextureShader
		shaderType = DECAL_TEXTURE_SHADER;
		shaderId   = m_ShaderProgramIdArray[BASIC_SHADER_BINARY_INDEX][shaderType];
		m_DecalTextureProgram.Initialize(shaderId);
		m_ShaderProgramPtrArray[shaderBinaryIndex][shaderType] = &m_DecalTextureProgram;

		return true;
	}

	bool ShaderManager::Finalize(void)
	{
		if (!m_Initialized)
		{
			return false;
		}

		for (u32 shaderBinaryIndex = 0; shaderBinaryIndex < MAX_SHADER_BINARY_NUM; shaderBinaryIndex++)
		{
			for (u32 shaderProgramIndex = 0; shaderProgramIndex < MAX_SHADER_PROGRAM_NUM; shaderProgramIndex++)
			{
				ProgramObject* programObjectPtr = m_ShaderProgramPtrArray[shaderBinaryIndex][shaderProgramIndex];
				if (programObjectPtr != NULL)
				{
					programObjectPtr->Finalize();
				}
			}
		}

		m_Initialized = false;

		return true;
	}

	u32 ShaderManager::GetVertexAttributes(const ShaderType shaderType)
	{
		ProgramObject* shaderProgramPtr = GetShaderProgram(shaderType);
		return shaderProgramPtr->GetVertexAttributes();
	}

	ProgramObject* ShaderManager::GetShaderProgram(const ShaderType shaderType)
	{
		if ((shaderType == COLOR_FILL_SHADER) || (shaderType == FONT_SHADER) || (shaderType == DECAL_TEXTURE_SHADER))
		{
			return m_ShaderProgramPtrArray[BASIC_SHADER_BINARY_INDEX][shaderType];
		}
		else
		{
			NN_TPANIC_("GetShaderProgram() failed. Invalid shaderType = %d\n", shaderType);
			return NULL;
		}
	}

}}	// namespace np::graphics
