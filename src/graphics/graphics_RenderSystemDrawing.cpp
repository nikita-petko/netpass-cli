#include <np/graphics/graphics_RenderSystemDrawing.h>

namespace np { namespace graphics {

	RenderSystemDrawing::RenderSystemDrawing(void)
	: RenderSystem()
	, m_InitializeTexturedTriangles(false)
	, m_UseDecalTextureProgram(false)
	, m_MaxTexturedTrianglesNum(graphics::MAX_TEXTURED_TRIANGLES_NUM)
	{
		for (u32 textureArrayIndex = 0; textureArrayIndex < graphics::MAX_TEXTURES_NUM; textureArrayIndex++)
		{
			m_TextureIdArray[textureArrayIndex]					= 0;
			m_TotalTexturedTrianglesNumArray[textureArrayIndex] = 0;
		}

		for (u32 displayIndex = 0; displayIndex < 3; displayIndex++)
		{
			for (u32 index = 0; index < 4; index++)
			{
				m_ClearColorArray[displayIndex][index] = 0.0f;
			}
		}
	}

	RenderSystemDrawing::~RenderSystemDrawing(void) { Finalize(); }

	void RenderSystemDrawing::Initialize(const uptr						  fcramAddress,
										 const size_t					  memorySize,
										 const u32						  commandBufferSize,
										 const u32						  requestNum,
										 const bool						  serialRunMode,
										 const DisplayBuffersDescription& displayBuffers0Desc,
										 const DisplayBuffersDescription& displayBuffers1Desc,
										 const FrameBufferDescription&	  frameBuffer0Desc,
										 const DisplayBuffersDescription& displayBuffers0ExtDesc,
										 const bool						  isFillBlackLCD)
	{
		RenderSystem::Initialize(fcramAddress,
								 memorySize,
								 commandBufferSize,
								 requestNum,
								 serialRunMode,
								 displayBuffers0Desc,
								 displayBuffers1Desc,
								 frameBuffer0Desc,
								 displayBuffers0ExtDesc,
								 isFillBlackLCD);

		m_InitializeTexturedTriangles = false;
		m_UseDecalTextureProgram	  = false;

		// NOTE: nngxInitialize must be called in advance. It is called from within RenderSystem::Initialize.
		m_GraphicsDrawing.Initialize();
	}

	void RenderSystemDrawing::InitializeTexturedTriangles(const u32 maxTrianglesNum)
	{
		if (m_InitializeTexturedTriangles)
		{
			return;
		}

		u32 vertexAttributes	  = m_GraphicsDrawing.m_ShaderManager.GetVertexAttributes(graphics::DECAL_TEXTURE_SHADER);
		u32 maxVerticesNum		  = 3 * maxTrianglesNum;
		m_MaxTexturedTrianglesNum = maxTrianglesNum;

		for (u32 textureArrayIndex = 0; textureArrayIndex < graphics::MAX_TEXTURES_NUM; textureArrayIndex++)
		{
			m_TotalTexturedTrianglesNumArray[textureArrayIndex] = 0;

			m_TexturedTrianglesRenderDataArray[textureArrayIndex].Initialize();
			m_TexturedTrianglesRenderDataArray[textureArrayIndex].InitializeVertexBuffers(vertexAttributes,
																						  GL_TRIANGLES,
																						  maxVerticesNum,
																						  m_MaxTexturedTrianglesNum);
		}

		for (u32 textureArrayIndex = 0; textureArrayIndex < graphics::MAX_TEXTURES_NUM; textureArrayIndex++)
		{
			m_TextureIdArray[textureArrayIndex] = 0;
		}

		m_InitializeTexturedTriangles = true;

		GRAPHICS_ASSERT_GL_ERROR();
	}

	void RenderSystemDrawing::InitializeTriangles(const u32 maxTrianglesNum) { m_GraphicsDrawing.InitializeTriangles(maxTrianglesNum); }

	void RenderSystemDrawing::InitializeSquares(const u32 maxSquaresNum) { m_GraphicsDrawing.InitializeSquares(maxSquaresNum); }

	void RenderSystemDrawing::InitializeTexts(const u32 maxLength) { m_GraphicsDrawing.InitializeTexts(maxLength); }

	void RenderSystemDrawing::Finalize(void)
	{
		for (u32 textureArrayIndex = 0; textureArrayIndex < MAX_TEXTURES_NUM; textureArrayIndex++)
		{
			m_TotalTexturedTrianglesNumArray[textureArrayIndex] = 0;
			m_TexturedTrianglesRenderDataArray[textureArrayIndex].Finalize();
		}
		m_InitializeTexturedTriangles = false;

		for (u32 textureArrayIndex = 0; textureArrayIndex < MAX_TEXTURES_NUM; textureArrayIndex++)
		{
			GLuint textureId = m_TextureIdArray[textureArrayIndex];
			if (textureId != 0)
			{
				glDeleteTextures(1, &textureId);
				m_TextureIdArray[textureArrayIndex] = 0;
			}
		}

		m_GraphicsDrawing.Finalize();
		RenderSystem::Finalize();

		GRAPHICS_ASSERT_GL_ERROR();
	}

	void RenderSystemDrawing::SetViewport(const GLint x, const GLint y, const GLsizei width, const GLsizei height)
	{
		NN_UNUSED_VAR(x);
		NN_UNUSED_VAR(y);

		m_GraphicsDrawing.SetWindowSize(width, height);

		for (u32 textureArrayIndex = 0; textureArrayIndex < MAX_TEXTURES_NUM; textureArrayIndex++)
		{
			m_TexturedTrianglesRenderDataArray[textureArrayIndex].SetWindowSize(width, height);
		}

		RenderSystem::SetViewport(x, y, width, height);
	}

	void RenderSystemDrawing::SetClearColor(const s32 display, const f32 red, const f32 green, const f32 blue, const f32 alpha)
	{
		if (display == NN_GX_DISPLAY0 || display == NN_GX_DISPLAY_BOTH)
		{
			m_ClearColorArray[0][0] = red;
			m_ClearColorArray[0][1] = green;
			m_ClearColorArray[0][2] = blue;
			m_ClearColorArray[0][3] = alpha;
		}

		if (display == NN_GX_DISPLAY1 || display == NN_GX_DISPLAY_BOTH)
		{
			m_ClearColorArray[1][0] = red;
			m_ClearColorArray[1][1] = green;
			m_ClearColorArray[1][2] = blue;
			m_ClearColorArray[1][3] = alpha;
		}
	}

	void RenderSystemDrawing::Clear(const GLbitfield mask)
	{
		u32 displayIndex = 0;
		if (m_TargetDisplay == NN_GX_DISPLAY0)
		{
			displayIndex = 0;
		}
		else if (m_TargetDisplay == NN_GX_DISPLAY1)
		{
			displayIndex = 1;
		}
		glClearColor(m_ClearColorArray[displayIndex][0],
					 m_ClearColorArray[displayIndex][1],
					 m_ClearColorArray[displayIndex][2],
					 m_ClearColorArray[displayIndex][3]);

		RenderSystem::Clear(mask);
	}

	void RenderSystemDrawing::SetColor(const f32 red, const f32 green, const f32 blue, const f32 alpha)
	{
		m_GraphicsDrawing.SetColor(red, green, blue, alpha);
	}

	void RenderSystemDrawing::SetColor(const f32 red, const f32 green, const f32 blue) { m_GraphicsDrawing.SetColor(red, green, blue); }

	void RenderSystemDrawing::SetDepth(const f32 depth) { m_GraphicsDrawing.SetDepth(depth); }

	void RenderSystemDrawing::SetAutoDepth(const bool enableAutoDepth) { m_GraphicsDrawing.SetAutoDepth(enableAutoDepth); }

	void RenderSystemDrawing::SetPointSize(const f32 pointSize) { m_GraphicsDrawing.SetPointSize(pointSize); }

	void RenderSystemDrawing::DrawPoint(const f32 windowCoordinateX, const f32 windowCoordinateY)
	{
		m_GraphicsDrawing.DrawPoint(windowCoordinateX, windowCoordinateY);
	}

	void RenderSystemDrawing::SetLineWidth(const f32 lineWidth) { m_GraphicsDrawing.SetLineWidth(lineWidth); }

	void RenderSystemDrawing::DrawLine(const f32 windowCoordinateX0,
									   const f32 windowCoordinateY0,
									   const f32 windowCoordinateX1,
									   const f32 windowCoordinateY1)
	{
		CheckRenderTarget();
		m_GraphicsDrawing.DrawLine(windowCoordinateX0, windowCoordinateY0, windowCoordinateX1, windowCoordinateY1);
	}

	void RenderSystemDrawing::FillSquare(const f32 windowCoordinateX0,
										 const f32 windowCoordinateY0,
										 const f32 windowCoordinateX1,
										 const f32 windowCoordinateY1,
										 const f32 windowCoordinateX2,
										 const f32 windowCoordinateY2,
										 const f32 windowCoordinateX3,
										 const f32 windowCoordinateY3)
	{
		CheckRenderTarget();
		m_GraphicsDrawing.FillSquare(windowCoordinateX0,
									 windowCoordinateY0,
									 windowCoordinateX1,
									 windowCoordinateY1,
									 windowCoordinateX2,
									 windowCoordinateY2,
									 windowCoordinateX3,
									 windowCoordinateY3);
	}

	void RenderSystemDrawing::FillRectangle(const f32 windowCoordinateX, const f32 windowCoordinateY, const f32 width, const f32 height)
	{
		CheckRenderTarget();
		m_GraphicsDrawing.FillRectangle(windowCoordinateX, windowCoordinateY, width, height);
	}

	void RenderSystemDrawing::FillTriangle(const f32 windowCoordinateX0,
										   const f32 windowCoordinateY0,
										   const f32 windowCoordinateX1,
										   const f32 windowCoordinateY1,
										   const f32 windowCoordinateX2,
										   const f32 windowCoordinateY2)
	{
		CheckRenderTarget();
		m_GraphicsDrawing.FillTriangle(windowCoordinateX0,
									   windowCoordinateY0,
									   windowCoordinateX1,
									   windowCoordinateY1,
									   windowCoordinateX2,
									   windowCoordinateY2);
	}

	void RenderSystemDrawing::SetFontSize(const f32 fontSize) { m_GraphicsDrawing.SetFontSize(fontSize); }

	void RenderSystemDrawing::DrawText(const f32 windowCoordinateX, const f32 windowCoordinateY, const char* format, ...)
	{
		CheckRenderTarget();

		static const u32 tmpBufferSize = 1024;
		static char		 textBuffer[tmpBufferSize];

		va_list va;
		va_start(va, format);
		vsnprintf(textBuffer, tmpBufferSize, format, va);
		va_end(va);

		m_GraphicsDrawing.DrawTextBuffer(windowCoordinateX, windowCoordinateY, textBuffer);
	}

	bool RenderSystemDrawing::HasTexture(const GLuint textureId)
	{
		if (!m_InitializeFlag)
		{
			NN_TPANIC_("Initialize() is not called.\n");
		}

		if (!m_InitializeTexturedTriangles)
		{
			InitializeTexturedTriangles();
		}

		u32 arrayIndex = 0;
		return FindTextureArrayIndex(textureId, arrayIndex);
	}

	bool RenderSystemDrawing::GenerateTexture(const GLenum	target,
											  const GLenum	internalFormat,
											  const GLsizei width,
											  const GLsizei height,
											  const GLenum	format,
											  const GLenum	type,
											  void*			pixels,
											  GLuint&		textureId)
	{
		if (!m_InitializeFlag)
		{
			NN_TPANIC_("Initialize() is not called.\n");
		}

		if (!m_InitializeTexturedTriangles)
		{
			InitializeTexturedTriangles();
		}

		u32 emptyTextureId	  = 0;
		u32 textureArrayIndex = 0;

		if (!FindTextureArrayIndex(emptyTextureId, textureArrayIndex))
		{
			NN_TPANIC_("Max loadable textures numbers is %d.\n", graphics::MAX_TEXTURES_NUM);

			return false;
		}

		glActiveTexture(GL_TEXTURE0);
		glGenTextures(1, &textureId);
		GRAPHICS_ASSERT_GL_ERROR();

		glBindTexture(GL_TEXTURE_2D, textureId);
		glTexImage2D(target, 0, internalFormat, width, height, 0, format, type, pixels);
		GRAPHICS_ASSERT_GL_ERROR();

		m_TextureIdArray[textureArrayIndex] = textureId;

		return true;
	}

	bool RenderSystemDrawing::DeleteTexture(const GLuint textureId)
	{
		if (!m_InitializeFlag)
		{
			NN_TPANIC_("Initialize() is not called.\n");
		}

		if (!m_InitializeTexturedTriangles)
		{
			InitializeTexturedTriangles();
		}

		if (textureId == 0)
		{
			return false;
		}

		u32 textureArrayIndex = 0;
		if (FindTextureArrayIndex(textureId, textureArrayIndex))
		{
			glDeleteTextures(1, &textureId);
			GRAPHICS_ASSERT_GL_ERROR();

			m_TextureIdArray[textureArrayIndex] = 0;

			return true;
		}
		else
		{
			return false;
		}
	}

	bool RenderSystemDrawing::FindTextureArrayIndex(const GLuint textureId, u32& arrayIndex)
	{
		if (!m_InitializeFlag)
		{
			NN_TPANIC_("Initialize() is not called.\n");
		}

		for (u32 textureArrayIndex = 0; textureArrayIndex < graphics::MAX_TEXTURES_NUM; textureArrayIndex++)
		{
			if (textureId == m_TextureIdArray[textureArrayIndex])
			{
				arrayIndex = textureArrayIndex;

				return true;
			}
		}

		return false;
	}

	void RenderSystemDrawing::FillTexturedRectangle(const GLuint textureId,
													const f32	 windowCoordinateX,
													const f32	 windowCoordinateY,
													const f32	 rectangleWidth,
													const f32	 rectangleHeight,
													const f32	 imageWidth,
													const f32	 imageHeight,
													const f32	 textureWidth,
													const f32	 textureHeight)
	{
		CheckRenderTarget();

		if ((textureId == 0) || (!HasTexture(textureId)))
		{
			NN_TPANIC_("Invalid textureId %d.\n", textureId);
			return;
		}

		f32 windowCoordinateX1 = windowCoordinateX + rectangleWidth;
		f32 windowCoordinateY1 = windowCoordinateY + rectangleHeight;

		f32 texcoordS0 = 0.0f;
		f32 texcoordT0 = 0.0f;
		f32 texcoordS1 = static_cast<f32>(imageWidth) / static_cast<f32>(textureWidth);
		f32 texcoordT1 = static_cast<f32>(imageHeight) / static_cast<f32>(textureHeight);

		FillTexturedTriangle(textureId,
							 windowCoordinateX,
							 windowCoordinateY,
							 texcoordS0,
							 texcoordT1,
							 windowCoordinateX,
							 windowCoordinateY1,
							 texcoordS0,
							 texcoordT0,
							 windowCoordinateX1,
							 windowCoordinateY1,
							 texcoordS1,
							 texcoordT0);

		FillTexturedTriangle(textureId,
							 windowCoordinateX,
							 windowCoordinateY,
							 texcoordS0,
							 texcoordT1,
							 windowCoordinateX1,
							 windowCoordinateY1,
							 texcoordS1,
							 texcoordT0,
							 windowCoordinateX1,
							 windowCoordinateY,
							 texcoordS1,
							 texcoordT1);
	}

	void RenderSystemDrawing::FillTexturedTriangle(const GLuint textureId,
												   const f32	windowCoordinateX0,
												   const f32	windowCoordinateY0,
												   const f32	texcoordS0,
												   const f32	texcoordT0,
												   const f32	windowCoordinateX1,
												   const f32	windowCoordinateY1,
												   const f32	texcoordS1,
												   const f32	texcoordT1,
												   const f32	windowCoordinateX2,
												   const f32	windowCoordinateY2,
												   const f32	texcoordS2,
												   const f32	texcoordT2)
	{
		CheckRenderTarget();

		if (!m_InitializeTexturedTriangles)
		{
			InitializeTexturedTriangles();
		}

		u32 textureArrayIndex = 0;
		if ((textureId == 0) || (!FindTextureArrayIndex(textureId, textureArrayIndex)))
		{
			NN_TPANIC_("Invalid textureId %d.\n", textureId);
			return;
		}

		if (m_TotalTexturedTrianglesNumArray[textureArrayIndex] >= m_MaxTexturedTrianglesNum)
		{
			NN_TLOG_("RenderSystemDrawing::FillTexturedTriangle() maxTriangles limit\n");
			NN_TLOG_("  trianglesIndex = %d, maxTrianglesNum = %d\n",
					 m_TotalTexturedTrianglesNumArray[textureArrayIndex],
					 m_MaxTexturedTrianglesNum);
			return;
		}

		m_UseDecalTextureProgram = true;

		graphics::TrianglesRenderData& renderData	 = m_TexturedTrianglesRenderDataArray[textureArrayIndex];
		u32							   triangleIndex = renderData.GetPackedTrianglesNum();

		f32 s_Depth = m_GraphicsDrawing.m_Depth;

		// POSITION0
		f32 normalizedDeviceCoordinateX = 0.0f;
		f32 normalizedDeviceCoordinateY = 0.0f;
		renderData.GetNormalizedDeviceCoordinateXY(windowCoordinateX0,
												   windowCoordinateY0,
												   normalizedDeviceCoordinateX,
												   normalizedDeviceCoordinateY);
		renderData.SetPosition(3 * triangleIndex, normalizedDeviceCoordinateX, normalizedDeviceCoordinateY, s_Depth, 1.0f);

		// POSITION1
		renderData.GetNormalizedDeviceCoordinateXY(windowCoordinateX1,
												   windowCoordinateY1,
												   normalizedDeviceCoordinateX,
												   normalizedDeviceCoordinateY);
		renderData.SetPosition(3 * triangleIndex + 1, normalizedDeviceCoordinateX, normalizedDeviceCoordinateY, s_Depth, 1.0f);

		// POSITION2
		renderData.GetNormalizedDeviceCoordinateXY(windowCoordinateX2,
												   windowCoordinateY2,
												   normalizedDeviceCoordinateX,
												   normalizedDeviceCoordinateY);
		renderData.SetPosition(3 * triangleIndex + 2, normalizedDeviceCoordinateX, normalizedDeviceCoordinateY, s_Depth, 1.0f);

		m_GraphicsDrawing.DecrementDepth();

		// TEXCOORD0-2
		renderData.SetTexcoord(3 * triangleIndex, texcoordS0, texcoordT0);
		renderData.SetTexcoord(3 * triangleIndex + 1, texcoordS1, texcoordT1);
		renderData.SetTexcoord(3 * triangleIndex + 2, texcoordS2, texcoordT2);

		// INDEX
		renderData.SetIndex(triangleIndex, 3 * triangleIndex, 3 * triangleIndex + 1, 3 * triangleIndex + 2);

		renderData.AddPackedVerticesNum(3);
		renderData.AddPackedTrianglesNum(1);

		m_TotalTexturedTrianglesNumArray[textureArrayIndex] += 1;
	}

	void RenderSystemDrawing::SwapBuffers(void)
	{
		CheckRenderTarget();

		RenderSystemDrawing::Flush();
		RenderSystem::SwapBuffers();
	}

	void RenderSystemDrawing::Flush(void)
	{
		FlushTexturedPrimitives();
		m_GraphicsDrawing.Flush();
	}

	void RenderSystemDrawing::FlushTexturedPrimitives(void)
	{
		if (!m_UseDecalTextureProgram)
		{
			return;
		}

		graphics::ProgramObject* shaderProgramPtr = m_GraphicsDrawing.m_ShaderManager.GetShaderProgram(graphics::DECAL_TEXTURE_SHADER);
		shaderProgramPtr->SetUse3d(false);
		shaderProgramPtr->Begin();
		shaderProgramPtr->Use();

		for (u32 textureArrayIndex = 0; textureArrayIndex < graphics::MAX_TEXTURES_NUM; textureArrayIndex++)
		{
			if (m_TextureIdArray[textureArrayIndex] != 0)
			{
				graphics::TrianglesRenderData& renderData		  = m_TexturedTrianglesRenderDataArray[textureArrayIndex];
				u32							   packedTrianglesNum = renderData.GetPackedTrianglesNum();
				if (packedTrianglesNum != 0)
				{
					GLuint textureId = m_TextureIdArray[textureArrayIndex];
					if (textureId != 0)
					{
						shaderProgramPtr->SetTextureId(textureId);
						shaderProgramPtr->Update();
						renderData.DrawPackedTriangles();
					}
				}

				m_TotalTexturedTrianglesNumArray[textureArrayIndex] = 0;
			}
		}

		shaderProgramPtr->End();

		m_UseDecalTextureProgram = false;
	}

}}	// namespace np::graphics
