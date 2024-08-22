#include <np/graphics/graphics_RenderSystemExt.h>

namespace np { namespace graphics {
	static GLenum s_CurrentLcdMode = NN_GX_DISPLAYMODE_NORMAL;
	static GLenum s_NextLcdMode	   = NN_GX_DISPLAYMODE_NORMAL;

	void RenderSystemExtVsyncCallback(GLenum targetDisplay)
	{
		if ((targetDisplay == NN_GX_DISPLAY0) && (s_CurrentLcdMode != s_NextLcdMode))
		{
			// Switch the LCD mode.
			nngxSetDisplayMode(s_NextLcdMode);

			if (s_NextLcdMode == NN_GX_DISPLAYMODE_STEREO)
			{
				nngxSwapBuffers(NN_GX_DISPLAY0);
			}

			s_CurrentLcdMode = s_NextLcdMode;
		}
	}

	RenderSystemExt::RenderSystemExt(void)
	: RenderSystemDrawing()
	, m_LcdMode(NN_GX_DISPLAYMODE_NORMAL)
	, m_DisplayBuffers0Ext()
	, m_ChangeLcdMode(false)
	{
	}

	RenderSystemExt::~RenderSystemExt(void) { Finalize(); }

	void RenderSystemExt::Initialize(const uptr						  fcramAddress,
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
		s_CurrentLcdMode = m_LcdMode;
		s_NextLcdMode	 = m_LcdMode;

		RenderSystemDrawing::Initialize(fcramAddress,
										memorySize,
										commandBufferSize,
										requestNum,
										serialRunMode,
										displayBuffers0Desc,
										displayBuffers1Desc,
										frameBuffer0Desc,
										displayBuffers0ExtDesc,
										isFillBlackLCD);
		DisplayBuffers::Create(displayBuffers0ExtDesc, m_DisplayBuffers0Ext);

		nngxSetVSyncCallback(NN_GX_DISPLAY0, RenderSystemExtVsyncCallback);

		m_InitializeFlag = true;
	}

	void RenderSystemExt::InitializeLcdDisplay(void)
	{
		// Initialize the upper and lower screen display buffers with black.
		// This function is called by the RenderSystem::Initialize function.
		glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
		SetRenderTarget(NN_GX_DISPLAY0);
		Clear();
		Transfer();
		SwapBuffers();

		SetRenderTarget(NN_GX_DISPLAY1);
		Clear();
		Transfer();
		SwapBuffers();

		SetRenderTarget(NN_GX_DISPLAY0);
		Clear();
		Transfer();
		SwapBuffers();

		SetRenderTarget(NN_GX_DISPLAY1);
		Clear();
		Transfer();
		SwapBuffers();

		WaitVsync(NN_GX_DISPLAY_BOTH);

		if (!m_InitializeLcdDisplayFlag)
		{
			nngxStartLcdDisplay();
			m_InitializeLcdDisplayFlag = true;
		}

		GRAPHICS_ASSERT_GL_ERROR();
	}

	void RenderSystemExt::Finalize(void)
	{
		DisplayBuffers::Destroy(m_DisplayBuffers0Ext);
		GRAPHICS_ASSERT_GL_ERROR();

		RenderSystemDrawing::Finalize();
	}

	void RenderSystemExt::SetLcdMode(const GLenum& lcdMode)
	{
		if ((lcdMode == NN_GX_DISPLAYMODE_NORMAL) || (lcdMode == NN_GX_DISPLAYMODE_STEREO))
		{
			if (m_LcdMode != lcdMode)
			{
				m_ChangeLcdMode = true;
				m_LcdMode		= lcdMode;
			}
		}
		else
		{
			NN_TPANIC_("Invalid lcd mode\n");
		}
	}

	GLenum RenderSystemExt::GetLcdMode(void) const { return m_LcdMode; }

	void RenderSystemExt::PrintLcdMode(void) const
	{
		if (m_LcdMode == NN_GX_DISPLAYMODE_NORMAL)
		{
			NN_LOG("Lcd mode: NN_GX_DISPLAYMODE_NORMAL\n");
		}
		else if (m_LcdMode == NN_GX_DISPLAYMODE_STEREO)
		{
			NN_LOG("Lcd mode: NN_GX_DISPLAYMODE_STEREO\n");
		}
		else
		{
			NN_TPANIC_("Invalid lcd mode\n");
		}
	}

	void RenderSystemExt::SetClearColor(const s32 display, const f32 red, const f32 green, const f32 blue, const f32 alpha)
	{
		if ((display == NN_GX_DISPLAY0) || (display == NN_GX_DISPLAY1) || (display == NN_GX_DISPLAY_BOTH))
		{
			RenderSystemDrawing::SetClearColor(display, red, green, blue, alpha);
		}

		if (display == NN_GX_DISPLAY0_EXT)
		{
			m_ClearColorArray[2][0] = red;
			m_ClearColorArray[2][1] = green;
			m_ClearColorArray[2][2] = blue;
			m_ClearColorArray[2][3] = alpha;
		}
	}

	void RenderSystemExt::Clear(const GLbitfield mask)
	{
		if ((m_TargetDisplay == NN_GX_DISPLAY0) || (m_TargetDisplay == NN_GX_DISPLAY1))
		{
			RenderSystemDrawing::Clear(mask);
		}
		else if (m_TargetDisplay == NN_GX_DISPLAY0_EXT)
		{
			u32 displayIndex = 2;
			glClearColor(m_ClearColorArray[displayIndex][0],
						 m_ClearColorArray[displayIndex][1],
						 m_ClearColorArray[displayIndex][2],
						 m_ClearColorArray[displayIndex][3]);

			RenderSystemDrawing::Clear(mask);
		}
	}

	void RenderSystemExt::SetRenderTarget(const GLenum targetDisplay)
	{
		if (!m_InitializeFlag)
		{
			NN_TPANIC_("Initialize() is not called.\n");
		}

		if (targetDisplay == NN_GX_DISPLAY0)
		{
			m_TargetDisplay			= targetDisplay;
			m_CurrentFrameBufferPtr = &m_FrameBuffer0;

			if ((m_LcdMode == NN_GX_DISPLAYMODE_NORMAL) || (m_LcdMode == NN_GX_DISPLAYMODE_STEREO))
			{
				m_CurrentDisplayBuffersPtr = &m_DisplayBuffers0;
			}
			else
			{
				NN_TPANIC_("Invalid lcd mode.\n");
			}
		}
		else if (targetDisplay == NN_GX_DISPLAY1)
		{
			m_TargetDisplay = targetDisplay;

			m_CurrentFrameBufferPtr	   = &m_FrameBuffer0;
			m_CurrentDisplayBuffersPtr = &m_DisplayBuffers1;
		}
		else if (targetDisplay == NN_GX_DISPLAY0_EXT)
		{
			if (m_LcdMode == NN_GX_DISPLAYMODE_STEREO)
			{
				m_TargetDisplay = targetDisplay;

				m_CurrentFrameBufferPtr	   = &m_FrameBuffer0;
				m_CurrentDisplayBuffersPtr = &m_DisplayBuffers0Ext;
			}
			else
			{
				NN_PANIC("LcdMode must be NN_GX_DISPLAYMODE_STEREO.\n");
			}
		}
		else
		{
			NN_TPANIC_("Invalid display.\n");
		}

		m_CurrentFrameBufferPtr->Bind();
		SetViewport(0, 0, m_CurrentDisplayBuffersPtr->GetWidth(), m_CurrentDisplayBuffersPtr->GetHeight());
	}

	GLuint RenderSystemExt::GetFramebufferObjectId(const GLenum targetDisplay)
	{
		if (!m_InitializeFlag)
		{
			NN_TPANIC_("Initialize() is not called.\n");
		}

		if ((targetDisplay == NN_GX_DISPLAY0) || (targetDisplay == NN_GX_DISPLAY1) || (targetDisplay == NN_GX_DISPLAY0_EXT))
		{
			return m_FrameBuffer0.GetFrameBufferId();
		}
		else
		{
			NN_TPANIC_("Invalid display.\n");

			return 0;
		}
	}

	GLuint RenderSystemExt::GetRenderbufferObjectId(const GLenum targetDisplay, const GLenum renderBufferType)
	{
		if (!m_InitializeFlag)
		{
			NN_TPANIC_("Initialize() is not called.\n");

			return 0;
		}

		if ((targetDisplay == NN_GX_DISPLAY0) || (targetDisplay == NN_GX_DISPLAY1) || (targetDisplay == NN_GX_DISPLAY0_EXT))
		{
			if (renderBufferType == GL_COLOR_ATTACHMENT0)
			{
				return m_FrameBuffer0.GetRenderBufferId(0);
			}
			else if (renderBufferType == GL_DEPTH_STENCIL_ATTACHMENT)
			{
				return m_FrameBuffer0.GetRenderBufferId(1);
			}
			else
			{
				NN_TPANIC_("Invalid renderBufferType.\n");

				return 0;
			}
		}
		else
		{
			NN_TPANIC_("Invalid display.\n");

			return 0;
		}
	}

	// In stereo display mode, after the image is transferred to both display buffers in the upper LCD, you must call
	// nngxSwapBuffers(NN_GX_DISPLAY0). Call the Transfer function to send the images to the display buffers.
	//
	//
	//
	void RenderSystemExt::Transfer(void)
	{
		if (!m_InitializeFlag)
		{
			return;
		}

		CheckRenderTarget();

		RenderSystemDrawing::Flush();

		// Transfer data from the color buffer to the display buffer.
		GLuint displayBufferId = m_CurrentDisplayBuffersPtr->GetTargetDisplayBufferId();
		// Get the ID of the display buffer data is being transferred to.
		m_CurrentDisplayBuffersPtr->Bind();
		m_CurrentFrameBufferPtr->Transfer(displayBufferId, NN_GX_ANTIALIASE_NOT_USED, GL_FALSE, 0, 0);
		GRAPHICS_ASSERT_GL_ERROR();

		// Execute the command list.
		m_CommandList.Run();
		m_CommandList.Swap();

		GRAPHICS_ASSERT_GL_ERROR();

		m_CurrentDisplayBuffersPtr->IncreaseBufferIndex();
	}

	// Send the image to the specified display buffer using the Transfer function and then call SwapBuffers.
	//
	void RenderSystemExt::SwapBuffers(void)
	{
		if (!m_InitializeFlag)
		{
			return;
		}

		if ((m_TargetDisplay == NN_GX_DISPLAY0) && (m_ChangeLcdMode) && (m_LcdMode == NN_GX_DISPLAYMODE_STEREO))
		{
			// If and only if you have switched to STEREO mode, temporarily bind the left-eye display buffer to the right-eye display.
			//
			GLuint displayBufferId = m_DisplayBuffers0.GetPreviousTargetDisplayBufferId();
			nngxActiveDisplay(NN_GX_DISPLAY0_EXT);
			nngxBindDisplaybuffer(displayBufferId);
		}

		if ((m_TargetDisplay == NN_GX_DISPLAY0) || (m_TargetDisplay == NN_GX_DISPLAY0_EXT))
		{
			nngxSwapBuffers(NN_GX_DISPLAY0);
		}
		else if (m_TargetDisplay == NN_GX_DISPLAY1)
		{
			nngxSwapBuffers(NN_GX_DISPLAY1);
		}

		GRAPHICS_ASSERT_GL_ERROR();
	}

	void RenderSystemExt::WaitVsync(const GLenum targetDisplay)
	{
		if (m_ChangeLcdMode)
		{
			s_NextLcdMode = m_LcdMode;
		}

		if ((targetDisplay == NN_GX_DISPLAY0) || (targetDisplay == NN_GX_DISPLAY1) || (targetDisplay == NN_GX_DISPLAY_BOTH))
		{
			nngxWaitVSync(targetDisplay);
		}
		else
		{
			NN_TPANIC_("Invalid display.\n");
		}

		if (m_ChangeLcdMode)
		{
			m_ChangeLcdMode = false;
		}
	}

	void RenderSystemExt::CheckRenderTarget(void)
	{
		if (!m_InitializeFlag)
		{
			NN_TPANIC_("Initialize() is not called.\n");
		}

		if (!((m_TargetDisplay == NN_GX_DISPLAY0) || (m_TargetDisplay == NN_GX_DISPLAY1) || (m_TargetDisplay == NN_GX_DISPLAY0_EXT)))
		{
			NN_TPANIC_("Invalid display. Call SetRenderTarget().\n");
		}

		if (m_CurrentDisplayBuffersPtr == NULL)
		{
			NN_TPANIC_("m_CurrentDisplayBufferPtr is NULL. Call SetRenderTarget().\n");
		}

		if (m_CurrentFrameBufferPtr == NULL)
		{
			NN_TPANIC_("m_CurrentFrameBufferPtr is NULL. Call SetRenderTarget().\n");
		}
	}

}}	// namespace np::graphics
