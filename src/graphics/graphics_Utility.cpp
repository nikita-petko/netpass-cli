#include <np/graphics/graphics_Utility.h>

namespace np { namespace graphics {
	/*!
	@brief Sends the error type to debug output if a GL error occurs and calls NN_TPANIC_.
	*/

	void PrintGLError(const GLenum errorCode)
	{
		if (errorCode != GL_NO_ERROR)
		{
			switch (errorCode)
			{
				case GL_INVALID_ENUM:
					NN_TLOG_("GL Error : GL_INVALID_ENUM\n");
					break;
				case GL_INVALID_VALUE:
					NN_TLOG_("GL Error : GL_INVALID_VALUE\n");
					break;
				case GL_INVALID_OPERATION:
					NN_TLOG_("GL Error : GL_INVALID_OPERATION\n");
					break;
				case GL_OUT_OF_MEMORY:
					NN_TLOG_("GL Error : GL_OUT_OF_MEMORY\n");
					break;
				case GL_INVALID_FRAMEBUFFER_OPERATION:
					NN_TLOG_("GL Error : GL_INVALID_FRAMEBUFFER_OPERATION\n");
					break;
				default:
					NN_TLOG_("GL Error : errorCode = 0x%x\n", errorCode);
					break;
			}
		}
	}

	/* ------------------------------------------------------------------------
		Utility Functions
		------------------------------------------------------------------------ */

	s32 s_UniformLocations[MAX_UNIFORM_LOCATIONS_NUM];

	void InitializeUniforms(const GLuint programId)
	{
		GRAPHICS_ASSERT_GL_ERROR();

		for (u32 uniformIndex = 0; uniformIndex < MAX_UNIFORM_LOCATIONS_NUM; uniformIndex++)
		{
			s_UniformLocations[uniformIndex] = -1;
		}

		// Vertex uniform.
		s_UniformLocations[VERTEX_UNIFORM_PROJECTION] = glGetUniformLocation(programId, "uProjection");
		s_UniformLocations[VERTEX_UNIFORM_MODELVIEW]  = glGetUniformLocation(programId, "uModelView");

		GRAPHICS_ASSERT_GL_ERROR();

		// Fragment uniform.
		s_UniformLocations[FRAGMENT_UNIFORM_ALPHA_TEST] = glGetUniformLocation(programId, "dmp_FragOperation.enableAlphaTest");

		// Fragment uniform: Texture samplerType.
		s_UniformLocations[FRAGMENT_UNIFORM_TEXTURE0_SAMPLER_TYPE] = glGetUniformLocation(programId, "dmp_Texture[0].samplerType");
		s_UniformLocations[FRAGMENT_UNIFORM_TEXTURE1_SAMPLER_TYPE] = glGetUniformLocation(programId, "dmp_Texture[1].samplerType");
		s_UniformLocations[FRAGMENT_UNIFORM_TEXTURE2_SAMPLER_TYPE] = glGetUniformLocation(programId, "dmp_Texture[2].samplerType");
		s_UniformLocations[FRAGMENT_UNIFORM_TEXTURE3_SAMPLER_TYPE] = glGetUniformLocation(programId, "dmp_Texture[3].samplerType");

		GRAPHICS_ASSERT_GL_ERROR();

		s_UniformLocations[FRAGMENT_UNIFORM_LIGHT_ENV_CONFIG]			= glGetUniformLocation(programId, "dmp_LightEnv.config");
		s_UniformLocations[FRAGMENT_UNIFORM_LIGHT_ENV_CLAMP_HIGHLIGHTS] = glGetUniformLocation(programId, "dmp_LightEnv.clampHighlights");
		s_UniformLocations[FRAGMENT_UNIFORM_LIGHT_ENV_LUT_ENABLED_REFL] = glGetUniformLocation(programId, "dmp_LightEnv.lutEnabledRefl");

		s_UniformLocations[FRAGMENT_UNIFORM_LIGHT_ENV_LUT_ENABLED_D0]	= glGetUniformLocation(programId, "dmp_LightEnv.lutEnabledD0");
		s_UniformLocations[FRAGMENT_UNIFORM_LIGHT_ENV_LUT_INPUT_D0]		= glGetUniformLocation(programId, "dmp_LightEnv.lutInputD0");
		s_UniformLocations[FRAGMENT_UNIFORM_LIGHT_ENV_ABS_LUT_INPUT_D0] = glGetUniformLocation(programId, "dmp_LightEnv.absLutInputD0");

		s_UniformLocations[FRAGMENT_UNIFORM_LIGHT_ENV_LUT_ENABLED_D1]	= glGetUniformLocation(programId, "dmp_LightEnv.lutEnabledD1");
		s_UniformLocations[FRAGMENT_UNIFORM_LIGHT_ENV_LUT_INPUT_D1]		= glGetUniformLocation(programId, "dmp_LightEnv.lutInputD1");
		s_UniformLocations[FRAGMENT_UNIFORM_LIGHT_ENV_ABS_LUT_INPUT_D1] = glGetUniformLocation(programId, "dmp_LightEnv.absLutInputD1");

		// Fragment uniform: Fragment lighting.
		s_UniformLocations[FRAGMENT_UNIFORM_FRAGMENT_LIGHTING_ENABLED] = glGetUniformLocation(programId, "dmp_FragmentLighting.enabled");
		s_UniformLocations[FRAGMENT_UNIFORM_FRAGMENT_LIGHTING_AMBIENT] = glGetUniformLocation(programId, "dmp_FragmentLighting.ambient");

		GRAPHICS_ASSERT_GL_ERROR();

		// Fragment uniform: Fragment light0.
		s_UniformLocations[FRAGMENT_UNIFORM_FRAGMENT_LIGHT_SOURCE0_ENABLED] =
			glGetUniformLocation(programId, "dmp_FragmentLightSource[0].enabled");
		s_UniformLocations[FRAGMENT_UNIFORM_FRAGMENT_LIGHT_SOURCE0_AMBIENT] =
			glGetUniformLocation(programId, "dmp_FragmentLightSource[0].ambient");
		s_UniformLocations[FRAGMENT_UNIFORM_FRAGMENT_LIGHT_SOURCE0_DIFFUSE] =
			glGetUniformLocation(programId, "dmp_FragmentLightSource[0].diffuse");
		s_UniformLocations[FRAGMENT_UNIFORM_FRAGMENT_LIGHT_SOURCE0_SPECULAR0] =
			glGetUniformLocation(programId, "dmp_FragmentLightSource[0].specular0");
		s_UniformLocations[FRAGMENT_UNIFORM_FRAGMENT_LIGHT_SOURCE0_SPECULAR1] =
			glGetUniformLocation(programId, "dmp_FragmentLightSource[0].specular1");
		s_UniformLocations[FRAGMENT_UNIFORM_FRAGMENT_LIGHT_SOURCE0_POSITION] =
			glGetUniformLocation(programId, "dmp_FragmentLightSource[0].position");
		s_UniformLocations[FRAGMENT_UNIFORM_FRAGMENT_LIGHT_SOURCE0_GEOM_FACTOR0] =
			glGetUniformLocation(programId, "dmp_FragmentLightSource[0].geomFactor0");
		s_UniformLocations[FRAGMENT_UNIFORM_FRAGMENT_LIGHT_SOURCE0_GEOM_FACTOR1] =
			glGetUniformLocation(programId, "dmp_FragmentLightSource[0].geomFactor1");
		s_UniformLocations[FRAGMENT_UNIFORM_FRAGMENT_LIGHT_SOURCE0_TWO_SIDE_DIFFUSE] =
			glGetUniformLocation(programId, "dmp_FragmentLightSource[0].twoSideDiffuse");

		GRAPHICS_ASSERT_GL_ERROR();

		// Fragment uniform: Fragment light1.
		s_UniformLocations[FRAGMENT_UNIFORM_FRAGMENT_LIGHT_SOURCE1_ENABLED] =
			glGetUniformLocation(programId, "dmp_FragmentLightSource[1].enabled");
		s_UniformLocations[FRAGMENT_UNIFORM_FRAGMENT_LIGHT_SOURCE1_AMBIENT] =
			glGetUniformLocation(programId, "dmp_FragmentLightSource[1].ambient");
		s_UniformLocations[FRAGMENT_UNIFORM_FRAGMENT_LIGHT_SOURCE1_DIFFUSE] =
			glGetUniformLocation(programId, "dmp_FragmentLightSource[1].diffuse");
		s_UniformLocations[FRAGMENT_UNIFORM_FRAGMENT_LIGHT_SOURCE1_SPECULAR0] =
			glGetUniformLocation(programId, "dmp_FragmentLightSource[1].specular0");
		s_UniformLocations[FRAGMENT_UNIFORM_FRAGMENT_LIGHT_SOURCE1_SPECULAR1] =
			glGetUniformLocation(programId, "dmp_FragmentLightSource[1].specular1");
		s_UniformLocations[FRAGMENT_UNIFORM_FRAGMENT_LIGHT_SOURCE1_POSITION] =
			glGetUniformLocation(programId, "dmp_FragmentLightSource[1].position");
		s_UniformLocations[FRAGMENT_UNIFORM_FRAGMENT_LIGHT_SOURCE1_GEOM_FACTOR0] =
			glGetUniformLocation(programId, "dmp_FragmentLightSource[1].geomFactor0");
		s_UniformLocations[FRAGMENT_UNIFORM_FRAGMENT_LIGHT_SOURCE1_GEOM_FACTOR1] =
			glGetUniformLocation(programId, "dmp_FragmentLightSource[1].geomFactor1");
		s_UniformLocations[FRAGMENT_UNIFORM_FRAGMENT_LIGHT_SOURCE1_TWO_SIDE_DIFFUSE] =
			glGetUniformLocation(programId, "dmp_FragmentLightSource[1].twoSideDiffuse");

		GRAPHICS_ASSERT_GL_ERROR();

		// Fragment uniform: Fragment light2.
		s_UniformLocations[FRAGMENT_UNIFORM_FRAGMENT_LIGHT_SOURCE2_ENABLED] =
			glGetUniformLocation(programId, "dmp_FragmentLightSource[2].enabled");
		s_UniformLocations[FRAGMENT_UNIFORM_FRAGMENT_LIGHT_SOURCE2_AMBIENT] =
			glGetUniformLocation(programId, "dmp_FragmentLightSource[2].ambient");
		s_UniformLocations[FRAGMENT_UNIFORM_FRAGMENT_LIGHT_SOURCE2_DIFFUSE] =
			glGetUniformLocation(programId, "dmp_FragmentLightSource[2].diffuse");
		s_UniformLocations[FRAGMENT_UNIFORM_FRAGMENT_LIGHT_SOURCE2_SPECULAR0] =
			glGetUniformLocation(programId, "dmp_FragmentLightSource[2].specular0");
		s_UniformLocations[FRAGMENT_UNIFORM_FRAGMENT_LIGHT_SOURCE2_SPECULAR1] =
			glGetUniformLocation(programId, "dmp_FragmentLightSource[2].specular1");
		s_UniformLocations[FRAGMENT_UNIFORM_FRAGMENT_LIGHT_SOURCE2_POSITION] =
			glGetUniformLocation(programId, "dmp_FragmentLightSource[2].position");
		s_UniformLocations[FRAGMENT_UNIFORM_FRAGMENT_LIGHT_SOURCE2_GEOM_FACTOR0] =
			glGetUniformLocation(programId, "dmp_FragmentLightSource[2].geomFactor0");
		s_UniformLocations[FRAGMENT_UNIFORM_FRAGMENT_LIGHT_SOURCE2_GEOM_FACTOR1] =
			glGetUniformLocation(programId, "dmp_FragmentLightSource[2].geomFactor1");
		s_UniformLocations[FRAGMENT_UNIFORM_FRAGMENT_LIGHT_SOURCE2_TWO_SIDE_DIFFUSE] =
			glGetUniformLocation(programId, "dmp_FragmentLightSource[2].twoSideDiffuse");

		GRAPHICS_ASSERT_GL_ERROR();

		// Fragment uniform: Fragment light3.
		s_UniformLocations[FRAGMENT_UNIFORM_FRAGMENT_LIGHT_SOURCE3_ENABLED] =
			glGetUniformLocation(programId, "dmp_FragmentLightSource[3].enabled");
		s_UniformLocations[FRAGMENT_UNIFORM_FRAGMENT_LIGHT_SOURCE3_AMBIENT] =
			glGetUniformLocation(programId, "dmp_FragmentLightSource[3].ambient");
		s_UniformLocations[FRAGMENT_UNIFORM_FRAGMENT_LIGHT_SOURCE3_DIFFUSE] =
			glGetUniformLocation(programId, "dmp_FragmentLightSource[3].diffuse");
		s_UniformLocations[FRAGMENT_UNIFORM_FRAGMENT_LIGHT_SOURCE3_SPECULAR0] =
			glGetUniformLocation(programId, "dmp_FragmentLightSource[3].specular0");
		s_UniformLocations[FRAGMENT_UNIFORM_FRAGMENT_LIGHT_SOURCE3_SPECULAR1] =
			glGetUniformLocation(programId, "dmp_FragmentLightSource[3].specular1");
		s_UniformLocations[FRAGMENT_UNIFORM_FRAGMENT_LIGHT_SOURCE3_POSITION] =
			glGetUniformLocation(programId, "dmp_FragmentLightSource[3].position");
		s_UniformLocations[FRAGMENT_UNIFORM_FRAGMENT_LIGHT_SOURCE3_GEOM_FACTOR0] =
			glGetUniformLocation(programId, "dmp_FragmentLightSource[3].geomFactor0");
		s_UniformLocations[FRAGMENT_UNIFORM_FRAGMENT_LIGHT_SOURCE3_GEOM_FACTOR1] =
			glGetUniformLocation(programId, "dmp_FragmentLightSource[3].geomFactor1");
		s_UniformLocations[FRAGMENT_UNIFORM_FRAGMENT_LIGHT_SOURCE3_TWO_SIDE_DIFFUSE] =
			glGetUniformLocation(programId, "dmp_FragmentLightSource[3].twoSideDiffuse");

		GRAPHICS_ASSERT_GL_ERROR();

		// Fragment uniform: Material
		s_UniformLocations[FRAGMENT_UNIFORM_FRAGMENT_MATERIAL_AMBIENT] = glGetUniformLocation(programId, "dmp_FragmentMaterial.ambient");
		s_UniformLocations[FRAGMENT_UNIFORM_FRAGMENT_MATERIAL_DIFFUSE] = glGetUniformLocation(programId, "dmp_FragmentMaterial.diffuse");
		s_UniformLocations[FRAGMENT_UNIFORM_FRAGMENT_MATERIAL_SPECULAR0] =
			glGetUniformLocation(programId, "dmp_FragmentMaterial.specular0");
		s_UniformLocations[FRAGMENT_UNIFORM_FRAGMENT_MATERIAL_SPECULAR1] =
			glGetUniformLocation(programId, "dmp_FragmentMaterial.specular1");
		s_UniformLocations[FRAGMENT_UNIFORM_FRAGMENT_MATERIAL_SAMPLER_D0] =
			glGetUniformLocation(programId, "dmp_FragmentMaterial.samplerD0");

		GRAPHICS_ASSERT_GL_ERROR();

		// Fragment uniform: Texture combiner0.
		s_UniformLocations[FRAGMENT_UNIFORM_TEXENV0_SRC_RGB]	   = glGetUniformLocation(programId, "dmp_TexEnv[0].srcRgb");
		s_UniformLocations[FRAGMENT_UNIFORM_TEXENV0_SRC_ALPHA]	   = glGetUniformLocation(programId, "dmp_TexEnv[0].srcAlpha");
		s_UniformLocations[FRAGMENT_UNIFORM_TEXENV0_OPERAND_RGB]   = glGetUniformLocation(programId, "dmp_TexEnv[0].operandRgb");
		s_UniformLocations[FRAGMENT_UNIFORM_TEXENV0_OPERAND_ALPHA] = glGetUniformLocation(programId, "dmp_TexEnv[0].operandAlpha");
		s_UniformLocations[FRAGMENT_UNIFORM_TEXENV0_COMBINE_RGB]   = glGetUniformLocation(programId, "dmp_TexEnv[0].combineRgb");
		s_UniformLocations[FRAGMENT_UNIFORM_TEXENV0_COMBINE_ALPHA] = glGetUniformLocation(programId, "dmp_TexEnv[0].combineAlpha");

		// Fragment uniform: Texture combiner1.
		s_UniformLocations[FRAGMENT_UNIFORM_TEXENV1_SRC_RGB]	   = glGetUniformLocation(programId, "dmp_TexEnv[1].srcRgb");
		s_UniformLocations[FRAGMENT_UNIFORM_TEXENV1_SRC_ALPHA]	   = glGetUniformLocation(programId, "dmp_TexEnv[1].srcAlpha");
		s_UniformLocations[FRAGMENT_UNIFORM_TEXENV1_OPERAND_RGB]   = glGetUniformLocation(programId, "dmp_TexEnv[1].operandRgb");
		s_UniformLocations[FRAGMENT_UNIFORM_TEXENV1_OPERAND_ALPHA] = glGetUniformLocation(programId, "dmp_TexEnv[1].operandAlpha");
		s_UniformLocations[FRAGMENT_UNIFORM_TEXENV1_COMBINE_RGB]   = glGetUniformLocation(programId, "dmp_TexEnv[1].combineRgb");
		s_UniformLocations[FRAGMENT_UNIFORM_TEXENV1_COMBINE_ALPHA] = glGetUniformLocation(programId, "dmp_TexEnv[1].combineAlpha");

		// Fragment uniform: Texture combiner2.
		s_UniformLocations[FRAGMENT_UNIFORM_TEXENV2_SRC_RGB]	   = glGetUniformLocation(programId, "dmp_TexEnv[2].srcRgb");
		s_UniformLocations[FRAGMENT_UNIFORM_TEXENV2_SRC_ALPHA]	   = glGetUniformLocation(programId, "dmp_TexEnv[2].srcAlpha");
		s_UniformLocations[FRAGMENT_UNIFORM_TEXENV2_OPERAND_RGB]   = glGetUniformLocation(programId, "dmp_TexEnv[2].operandRgb");
		s_UniformLocations[FRAGMENT_UNIFORM_TEXENV2_OPERAND_ALPHA] = glGetUniformLocation(programId, "dmp_TexEnv[2].operandAlpha");
		s_UniformLocations[FRAGMENT_UNIFORM_TEXENV2_COMBINE_RGB]   = glGetUniformLocation(programId, "dmp_TexEnv[2].combineRgb");
		s_UniformLocations[FRAGMENT_UNIFORM_TEXENV2_COMBINE_ALPHA] = glGetUniformLocation(programId, "dmp_TexEnv[2].combineAlpha");

		GRAPHICS_ASSERT_GL_ERROR();
	}

	void GetRandomColor(f32& red, f32& green, f32& blue)
	{
		static bool s_InitializeRandomSeed = false;
		if (!s_InitializeRandomSeed)
		{
			::std::srand(0x1234);
			s_InitializeRandomSeed = true;
		}

		red	  = static_cast<f32>(::std::rand() % 256) / 256.0f;
		green = static_cast<f32>(::std::rand() % 256) / 256.0f;
		blue  = static_cast<f32>(::std::rand() % 256) / 256.0f;
	}

}}	// namespace np::graphics
