#include <np/graphics/graphics_GraphicsDrawing.h>

namespace np { namespace graphics {

	/* ------------------------------------------------------------------------
		GraphicsDrawing Class Member Function
	------------------------------------------------------------------------ */

	GraphicsDrawing::GraphicsDrawing(void)
	: m_Depth(1.0f)
	, m_AutoDepth(true)
	, m_ShaderManager()
	, m_InitializeTriangles(false)
	, m_InitializeSquares(false)
	, m_InitializeTexts(false)
	, m_MaxTrianglesNum(graphics::MAX_TRIANGLES_NUM)
	, m_TotalTrianglesNum(0)
	, m_MaxSquaresNum(graphics::MAX_SQUARES_NUM)
	, m_TotalSquaresNum(0)
	, m_MaxTextLength(graphics::MAX_TEXT_LENGTH)
	, m_TotalTextLength(0)
	, m_UseColorFillProgram(false)
	, m_UseFontProgram(false)
	, m_PointSize(1.0f)
	, m_LineWidth(1.0f)
	, m_FontSize(1.0f)
	{
		SetWindowSize(NN_GX_DISPLAY0_WIDTH, NN_GX_DISPLAY0_HEIGHT);
	}

	GraphicsDrawing::~GraphicsDrawing(void) { Finalize(); }

	void GraphicsDrawing::Initialize(void)
	{
		m_InitializeTriangles = false;
		m_InitializeSquares	  = false;
		m_InitializeTexts	  = false;

		m_UseColorFillProgram = false;
		m_UseFontProgram	  = false;

		m_Color[0] = 1.0f;
		m_Color[1] = 0.0f;
		m_Color[2] = 0.0f;
		m_Color[3] = 1.0f;

		m_Depth		 = 1.0f;
		m_AutoDepth	 = true;
		m_DepthValue = 0.0001f;

		DecrementDepth();

		m_PointSize = 1.0f;
		m_LineWidth = 1.0f;
		m_FontSize	= 8.0f;

		bool flag = m_ShaderManager.Initialize();
		if (!flag)
		{
			NN_TPANIC_("GraphicsDrawing::Initialize() : ShaderManager.Initialize() failed.\n");
		}
	}

	void GraphicsDrawing::Finalize(void)
	{
		GRAPHICS_ASSERT_GL_ERROR();

		m_ShaderManager.Finalize();

		m_TrianglesRenderData.Finalize();
		m_SquaresRenderData.Finalize();
		m_TextsRenderData.Finalize();

		m_InitializeTriangles = false;
		m_InitializeSquares	  = false;
		m_InitializeTexts	  = false;

		GRAPHICS_ASSERT_GL_ERROR();
	}

	void GraphicsDrawing::InitializeTriangles(const u32 maxTrianglesNum)
	{
		if (m_InitializeTriangles)
		{
			return;
		}

		GRAPHICS_ASSERT_GL_ERROR();

		m_TrianglesRenderData.Initialize();

		u32 vertexAttributes = m_ShaderManager.GetVertexAttributes(graphics::COLOR_FILL_SHADER);

		m_MaxTrianglesNum  = maxTrianglesNum;
		u32 maxVerticesNum = 3 * maxTrianglesNum;
		m_TrianglesRenderData.InitializeVertexBuffers(vertexAttributes, GL_TRIANGLES, maxVerticesNum, m_MaxTrianglesNum);
		m_TotalTrianglesNum = 0;

		m_InitializeTriangles = true;

		GRAPHICS_ASSERT_GL_ERROR();
	}

	void GraphicsDrawing::InitializeSquares(const u32 maxSquaresNum)
	{
		if (m_InitializeSquares)
		{
			return;
		}

		GRAPHICS_ASSERT_GL_ERROR();

		m_SquaresRenderData.Initialize();

		u32 vertexAttributes = m_ShaderManager.GetVertexAttributes(graphics::COLOR_FILL_SHADER);

		m_MaxSquaresNum		= maxSquaresNum;
		u32 maxVerticesNum	= m_MaxSquaresNum * 4;
		u32 maxTrianglesNum = m_MaxSquaresNum * 6;
		m_SquaresRenderData.InitializeVertexBuffers(vertexAttributes, GL_TRIANGLE_STRIP, maxVerticesNum, maxTrianglesNum);
		m_TotalSquaresNum = 0;

		m_InitializeSquares = true;

		GRAPHICS_ASSERT_GL_ERROR();
	}

	void GraphicsDrawing::InitializeTexts(const u32 maxLength)
	{
		if (m_InitializeTexts)
		{
			return;
		}

		GRAPHICS_ASSERT_GL_ERROR();

		m_TextsRenderData.Initialize();

		u32 vertexAttributes = m_ShaderManager.GetVertexAttributes(graphics::FONT_SHADER);

		m_MaxTextLength = maxLength;
		m_TextsRenderData.InitializeVertexBuffers(vertexAttributes, GL_TRIANGLES, m_MaxTextLength);
		m_TotalTextLength = 0;

		m_InitializeTexts = true;

		GRAPHICS_ASSERT_GL_ERROR();
	}

	void GraphicsDrawing::SetWindowSize(const f32 windowWidth, const f32 windowHeight)
	{
		m_WindowWidth  = windowWidth;
		m_WindowHeight = windowHeight;

		m_TrianglesRenderData.SetWindowSize(m_WindowWidth, m_WindowHeight);
		m_SquaresRenderData.SetWindowSize(m_WindowWidth, m_WindowHeight);
		m_TextsRenderData.SetWindowSize(m_WindowWidth, m_WindowHeight);
	}

	void GraphicsDrawing::GetWindowSize(f32& windowWidth, f32& windowHeight)
	{
		windowWidth	 = m_WindowWidth;
		windowHeight = m_WindowHeight;
	}

	void GraphicsDrawing::SetColor(const f32 red, const f32 green, const f32 blue)
	{
		m_Color[0] = red;
		m_Color[1] = green;
		m_Color[2] = blue;
		m_Color[3] = 1.0f;
	}

	void GraphicsDrawing::SetColor(const f32 red, const f32 green, const f32 blue, const f32 alpha)
	{
		m_Color[0] = red;
		m_Color[1] = green;
		m_Color[2] = blue;
		m_Color[3] = alpha;
	}

	void GraphicsDrawing::SetDepth(const f32 depth) { m_Depth = depth; }

	void GraphicsDrawing::SetAutoDepth(const bool enableAutoDepth) { m_AutoDepth = enableAutoDepth; }

	void GraphicsDrawing::DecrementDepth(void)
	{
		if (m_AutoDepth)
		{
			m_Depth -= m_DepthValue;
		}

		if (m_Depth <= 0.0f)
		{
			m_Depth = 0.0f;
		}
	}

	void GraphicsDrawing::SetPointSize(const f32 pointSize) { m_PointSize = pointSize; }

	void GraphicsDrawing::DrawPoint(const f32 windowCoordinateX, const f32 windowCoordinateY)
	{
		f32 pointHalfSize = 0.5f * m_PointSize;

		f32 windowCoordinateX0 = windowCoordinateX - pointHalfSize;
		f32 windowCoordinateX1 = windowCoordinateX + pointHalfSize;
		f32 windowCoordinateY0 = windowCoordinateY - pointHalfSize;
		f32 windowCoordinateY1 = windowCoordinateY + pointHalfSize;

		FillSquare(windowCoordinateX0,
				   windowCoordinateY0,
				   windowCoordinateX0,
				   windowCoordinateY1,
				   windowCoordinateX1,
				   windowCoordinateY1,
				   windowCoordinateX1,
				   windowCoordinateY0);
	}

	void GraphicsDrawing::SetLineWidth(const f32 lineWidth) { m_LineWidth = lineWidth; }

	void GraphicsDrawing::DrawLine(const f32 windowCoordinateX0,
								   const f32 windowCoordinateY0,
								   const f32 windowCoordinateX1,
								   const f32 windowCoordinateY1)
	{
		static const f32 GRAPHICS_MATH_ERROR = 0.00001f;

		f32 s_LineHalfWidth = 0.5f * m_LineWidth;

		f32 x01 = windowCoordinateX1 - windowCoordinateX0;
		f32 y01 = windowCoordinateY1 - windowCoordinateY0;

		f32 gradientX = 0.0f;
		f32 gradientY = 0.0f;

		if (nn::math::FAbs(x01) < GRAPHICS_MATH_ERROR)
		{
			gradientX = 1.0f;
		}
		else if (nn::math::FAbs(y01) < GRAPHICS_MATH_ERROR)
		{
			gradientY = 1.0f;
		}
		else
		{
			gradientX = y01;
			gradientY = (-x01);

			f32 gradient = gradientX * gradientX + gradientY * gradientY;
			gradient	 = nn::math::FSqrt(gradient);

			gradientX /= gradient;
			gradientY /= gradient;
		}

		gradientX *= s_LineHalfWidth;
		gradientY *= s_LineHalfWidth;

		FillSquare(windowCoordinateX0 + gradientX,
				   windowCoordinateY0 + gradientY,
				   windowCoordinateX0 - gradientX,
				   windowCoordinateY0 - gradientY,
				   windowCoordinateX1 - gradientX,
				   windowCoordinateY1 - gradientY,
				   windowCoordinateX1 + gradientX,
				   windowCoordinateY1 + gradientY);

		GRAPHICS_ASSERT_GL_ERROR();
	}

	void GraphicsDrawing::FillSquare(const f32 windowCoordinateX0,
									 const f32 windowCoordinateY0,
									 const f32 windowCoordinateX1,
									 const f32 windowCoordinateY1,
									 const f32 windowCoordinateX2,
									 const f32 windowCoordinateY2,
									 const f32 windowCoordinateX3,
									 const f32 windowCoordinateY3)
	{
		if (!m_InitializeSquares)
		{
			InitializeSquares();
		}

		if (m_TotalSquaresNum >= m_MaxSquaresNum)
		{
			NN_TLOG_("GraphicsDrawing::FillSquare() maxSquares limit\n");
			NN_TLOG_("  squaresIndex = %d, maxSquaresNum = %d\n", m_TotalSquaresNum, m_MaxSquaresNum);
			return;
		}

		m_UseColorFillProgram = true;

		graphics::TrianglesRenderData& renderData  = m_SquaresRenderData;
		u32							   vertexIndex = renderData.GetPackedVerticesNum();
		u32							   squareIndex = renderData.GetPackedTrianglesNum() / 4;

		// POSITION 0
		f32 normalizedDeviceCoordinateX = 0.0f;
		f32 normalizedDeviceCoordinateY = 0.0f;
		renderData.GetNormalizedDeviceCoordinateXY(windowCoordinateX0,
												   windowCoordinateY0,
												   normalizedDeviceCoordinateX,
												   normalizedDeviceCoordinateY);
		renderData.SetPosition(vertexIndex, normalizedDeviceCoordinateX, normalizedDeviceCoordinateY, m_Depth, 1.0f);

		// POSITION 1
		renderData.GetNormalizedDeviceCoordinateXY(windowCoordinateX1,
												   windowCoordinateY1,
												   normalizedDeviceCoordinateX,
												   normalizedDeviceCoordinateY);
		renderData.SetPosition(vertexIndex + 2, normalizedDeviceCoordinateX, normalizedDeviceCoordinateY, m_Depth, 1.0f);

		// POSITION 2
		renderData.GetNormalizedDeviceCoordinateXY(windowCoordinateX2,
												   windowCoordinateY2,
												   normalizedDeviceCoordinateX,
												   normalizedDeviceCoordinateY);
		renderData.SetPosition(vertexIndex + 3, normalizedDeviceCoordinateX, normalizedDeviceCoordinateY, m_Depth, 1.0f);

		// POSITION 3
		renderData.GetNormalizedDeviceCoordinateXY(windowCoordinateX3,
												   windowCoordinateY3,
												   normalizedDeviceCoordinateX,
												   normalizedDeviceCoordinateY);
		renderData.SetPosition(vertexIndex + 1, normalizedDeviceCoordinateX, normalizedDeviceCoordinateY, m_Depth, 1.0f);

		DecrementDepth();

		// COLOR
		for (u32 i = 0; i < 4; i++)
		{
			renderData.SetColor(vertexIndex + i, m_Color[0], m_Color[1], m_Color[2]);
		}

		// INDEX
		renderData.SetSquareIndex(squareIndex, vertexIndex, vertexIndex + 1, vertexIndex + 2, vertexIndex + 3);

		renderData.AddPackedVerticesNum(4);
		renderData.AddPackedTrianglesNum(4);

		m_TotalSquaresNum += 1;

		GRAPHICS_ASSERT_GL_ERROR();
	}

	void GraphicsDrawing::FillRectangle(const f32 windowCoordinateX, const f32 windowCoordinateY, const f32 width, const f32 height)
	{
		f32 windowCoordinateX1 = windowCoordinateX + width;
		f32 windowCoordinateY1 = windowCoordinateY + height;

		FillTriangle(windowCoordinateX, windowCoordinateY, windowCoordinateX, windowCoordinateY1, windowCoordinateX1, windowCoordinateY);

		FillTriangle(windowCoordinateX, windowCoordinateY1, windowCoordinateX1, windowCoordinateY1, windowCoordinateX1, windowCoordinateY);

		GRAPHICS_ASSERT_GL_ERROR();
	}

	void GraphicsDrawing::FillTriangle(const f32 windowCoordinateX0,
									   const f32 windowCoordinateY0,
									   const f32 windowCoordinateX1,
									   const f32 windowCoordinateY1,
									   const f32 windowCoordinateX2,
									   const f32 windowCoordinateY2)
	{
		if (!m_InitializeTriangles)
		{
			InitializeTriangles();
		}

		if (m_TotalTrianglesNum >= m_MaxTrianglesNum)
		{
			NN_TLOG_("GraphicsDrawing::FillTriangle() maxSquares limit\n");
			NN_TLOG_("  trianglesIndex = %d, maxTrianglesNum = %d\n", m_TotalTrianglesNum, m_MaxTrianglesNum);
			return;
		}

		m_UseColorFillProgram						 = true;
		graphics::TrianglesRenderData& renderData	 = m_TrianglesRenderData;
		u32							   triangleIndex = renderData.GetPackedTrianglesNum();

		// POSITION 0
		f32 normalizedDeviceCoordinateX = 0.0f;
		f32 normalizedDeviceCoordinateY = 0.0f;
		renderData.GetNormalizedDeviceCoordinateXY(windowCoordinateX0,
												   windowCoordinateY0,
												   normalizedDeviceCoordinateX,
												   normalizedDeviceCoordinateY);
		renderData.SetPosition(3 * triangleIndex, normalizedDeviceCoordinateX, normalizedDeviceCoordinateY, m_Depth, 1.0f);

		// POSITION 1
		renderData.GetNormalizedDeviceCoordinateXY(windowCoordinateX1,
												   windowCoordinateY1,
												   normalizedDeviceCoordinateX,
												   normalizedDeviceCoordinateY);
		renderData.SetPosition(3 * triangleIndex + 1, normalizedDeviceCoordinateX, normalizedDeviceCoordinateY, m_Depth, 1.0f);

		// POSITION 2
		renderData.GetNormalizedDeviceCoordinateXY(windowCoordinateX2,
												   windowCoordinateY2,
												   normalizedDeviceCoordinateX,
												   normalizedDeviceCoordinateY);
		renderData.SetPosition(3 * triangleIndex + 2, normalizedDeviceCoordinateX, normalizedDeviceCoordinateY, m_Depth, 1.0f);

		DecrementDepth();

		// COLOR
		for (u32 i = 0; i < 3; i++)
		{
			renderData.SetColor(3 * triangleIndex + i, m_Color[0], m_Color[1], m_Color[2]);
		}

		// INDEX
		renderData.SetIndex(triangleIndex, 3 * triangleIndex, 3 * triangleIndex + 1, 3 * triangleIndex + 2);

		renderData.AddPackedVerticesNum(3);
		renderData.AddPackedTrianglesNum(1);

		m_TotalTrianglesNum += 1;

		GRAPHICS_ASSERT_GL_ERROR();
	}

	void GraphicsDrawing::SetFontSize(const f32 fontSize)
	{
		m_FontSize = fontSize;
		m_TextsRenderData.SetFontSize(m_FontSize);
	}

	void GraphicsDrawing::DrawText(const f32 windowCoordinateX, const f32 windowCoordinateY, const char* format, ...)
	{
		static const u32 tmpBufferSize = 1024;
		static char		 textBuffer[tmpBufferSize];

		if (!m_InitializeTexts)
		{
			InitializeTexts();
		}

		va_list va;
		va_start(va, format);
		vsnprintf(textBuffer, tmpBufferSize, format, va);
		va_end(va);

		DrawTextBuffer(windowCoordinateX, windowCoordinateY, textBuffer);

		GRAPHICS_ASSERT_GL_ERROR();
	}

	void GraphicsDrawing::DrawTextBuffer(const f32 windowCoordinateX, const f32 windowCoordinateY, const char* textBuffer)
	{
		if (!m_InitializeTexts)
		{
			InitializeTexts();
		}

		char* ch_ptr	 = const_cast<char*>(textBuffer);
		u32	  textLength = 0;
		for (u32 i = 0; i < graphics::MAX_CHAR_LENGTH_PER_LINE; i++)
		{
			char c = (*ch_ptr);
			if (c == '\0')
			{
				break;
			}

			if ((0x20 <= c) && (c <= 0x7f))
			{
				textLength += 1;
				ch_ptr++;
			}
			else
			{
				break;
			}
		}

		graphics::TextsRenderData& renderData = m_TextsRenderData;
		if ((m_TotalTextLength + textLength) >= m_MaxTextLength)
		{
			NN_TLOG_("GraphicsDrawing::DrawText() maxText limit\n");
			NN_TLOG_("  textLength = %d, maxTextLength = %d, totalTextLength = %d\n", textLength, m_MaxTextLength, m_TotalTextLength);
			return;
		}

		m_UseFontProgram = true;

		m_TextsRenderData.SetFontColor(m_Color[0], m_Color[1], m_Color[2], m_Color[3]);
		renderData.AddText(windowCoordinateX, windowCoordinateY, textBuffer, textLength, m_Depth);

		m_TotalTextLength += textLength;

		DecrementDepth();

		GRAPHICS_ASSERT_GL_ERROR();
	}

	void GraphicsDrawing::Flush(void)
	{
		FlushColorFillPrimitives();
		FlushTexts();

		m_Depth = 1.0f;
		DecrementDepth();
	}

	void GraphicsDrawing::FlushColorFillPrimitives(void)
	{
		if (!m_UseColorFillProgram)
		{
			return;
		}

		graphics::ProgramObject* shaderProgramPtr = m_ShaderManager.GetShaderProgram(graphics::COLOR_FILL_SHADER);
		shaderProgramPtr->SetUse3d(false);
		shaderProgramPtr->Begin();
		shaderProgramPtr->Use();

		m_TrianglesRenderData.DrawPackedTriangles();
		m_SquaresRenderData.DrawPackedTriangles();

		m_TotalTrianglesNum = 0;
		m_TotalSquaresNum	= 0;

		shaderProgramPtr->End();

		m_UseColorFillProgram = false;

		GRAPHICS_ASSERT_GL_ERROR();
	}

	void GraphicsDrawing::FlushTexts(void)
	{
		if (!m_UseFontProgram)
		{
			return;
		}

		graphics::ProgramObject* shaderProgramPtr = m_ShaderManager.GetShaderProgram(graphics::FONT_SHADER);
		shaderProgramPtr->SetUse3d(false);
		shaderProgramPtr->Begin();
		shaderProgramPtr->Use();

		m_TextsRenderData.DrawPackedTriangles();
		m_TextsRenderData.ClearPackedTextLength();
		m_TotalTextLength = 0;

		shaderProgramPtr->End();

		m_UseFontProgram = false;

		GRAPHICS_ASSERT_GL_ERROR();
	}

}}	// namespace np::graphics
