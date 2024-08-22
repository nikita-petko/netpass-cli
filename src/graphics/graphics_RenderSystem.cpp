#include <np/graphics/graphics_RenderSystem.h>
#include <nn/applet.h>

namespace np { namespace graphics {

	RenderSystem::RenderSystem(void)
	: m_InitializeFlag(false)
	, m_InitializeLcdDisplayFlag(false)
	, m_TargetDisplay(NN_GX_DISPLAY0)
	, m_CurrentDisplayBuffersPtr(NULL)
	, m_CurrentFrameBufferPtr(NULL)
	, m_DisplayBuffers0()
	, m_FrameBuffer0()
	, m_DisplayBuffers1()
	{
	}

	RenderSystem::~RenderSystem(void) { Finalize(); }

	void RenderSystem::Initialize(const uptr					   fcramAddress,
								  const size_t					   memorySize,
								  const u32						   commandBufferSize,
								  const u32						   requestNum,
								  const bool					   serialRunMode,
								  const DisplayBuffersDescription& displayBuffers0Desc,
								  const DisplayBuffersDescription& displayBuffers1Desc,
								  const FrameBufferDescription&	   frameBuffer0Desc,
								  const DisplayBuffersDescription& displayBuffers0ExtDesc,
								  const bool					   isFillBlackLCD)
	{
		NN_UNUSED_VAR(displayBuffers0ExtDesc);
		GRAPHICS_FORCE_LINK_APPLET_LIB();

		if (m_InitializeFlag)
		{
			NN_TPANIC_("Initialize() is called twice.\n");
		}

		graphics::InitializeMemoryManager(fcramAddress, memorySize);
		if (nngxInitialize(graphics::GetAllocator, graphics::GetDeallocator) == GL_FALSE)
		{
			NN_TPANIC_("nngxInitialize() failed.\n");
		}

		m_CommandList.Initialize(commandBufferSize, requestNum, serialRunMode);

		DisplayBuffers::Create(displayBuffers0Desc, m_DisplayBuffers0);
		GRAPHICS_ASSERT_GL_ERROR();

		DisplayBuffers::Create(displayBuffers1Desc, m_DisplayBuffers1);
		GRAPHICS_ASSERT_GL_ERROR();

		FrameBuffer::Create(frameBuffer0Desc, m_FrameBuffer0);
		GRAPHICS_ASSERT_GL_ERROR();

		m_InitializeFlag = true;

		if (isFillBlackLCD)
			InitializeLcdDisplay();

		GRAPHICS_ASSERT_GL_ERROR();
	}

	void RenderSystem::InitializeLcdDisplay(void)
	{
		// Initialize the upper and lower screen display buffers with black.
		glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
		SetRenderTarget(NN_GX_DISPLAY0);
		Clear();
		SwapBuffers();

		SetRenderTarget(NN_GX_DISPLAY1);
		Clear();
		SwapBuffers();

		SetRenderTarget(NN_GX_DISPLAY0);
		Clear();
		SwapBuffers();

		SetRenderTarget(NN_GX_DISPLAY1);
		Clear();
		SwapBuffers();

		WaitVsync(NN_GX_DISPLAY_BOTH);

		if (!m_InitializeLcdDisplayFlag)
		{
			nngxStartLcdDisplay();
			m_InitializeLcdDisplayFlag = true;
		}
	}

	void RenderSystem::Finalize(void)
	{
		if (!m_InitializeFlag)
		{
			return;
		}

		GRAPHICS_ASSERT_GL_ERROR();

		DisplayBuffers::Destroy(m_DisplayBuffers0);
		DisplayBuffers::Destroy(m_DisplayBuffers1);
		FrameBuffer::Destroy(m_FrameBuffer0);

		m_CommandList.Stop();
		GRAPHICS_ASSERT_GL_ERROR();

		m_CommandList.Finalize();
		GRAPHICS_ASSERT_GL_ERROR();

		nngxFinalize();
		GRAPHICS_ASSERT_GL_ERROR();

		graphics::FinalizeMemoryManager();
		GRAPHICS_ASSERT_GL_ERROR();

		m_InitializeFlag = false;
	}

	void RenderSystem::SetRenderTarget(const GLenum targetDisplay)
	{
		if (!m_InitializeFlag)
		{
			NN_TPANIC_("Initialize() is not called.\n");
		}

		if (targetDisplay == NN_GX_DISPLAY0)
		{
			m_TargetDisplay = targetDisplay;

			m_CurrentFrameBufferPtr	   = &m_FrameBuffer0;
			m_CurrentDisplayBuffersPtr = &m_DisplayBuffers0;
		}
		else if (targetDisplay == NN_GX_DISPLAY1)
		{
			m_TargetDisplay = targetDisplay;

			m_CurrentFrameBufferPtr	   = &m_FrameBuffer0;
			m_CurrentDisplayBuffersPtr = &m_DisplayBuffers1;
		}
		else
		{
			NN_TPANIC_("Invalid display name.\n");
		}

		m_CurrentFrameBufferPtr->Bind();
		SetViewport(0, 0, m_CurrentDisplayBuffersPtr->GetWidth(), m_CurrentDisplayBuffersPtr->GetHeight());

		CheckRenderTarget();
	}

	void RenderSystem::SetViewport(const GLint x, const GLint y, const GLsizei width, const GLsizei height)
	{
		glViewport(x, y, width, height);
	}

	void RenderSystem::Clear(const GLbitfield mask)
	{
		CheckRenderTarget();

		m_CurrentFrameBufferPtr->ClearBuffer(mask);
	}

	void RenderSystem::ClearColorDepthStencilBuffer(const GLclampf red,
													const GLclampf green,
													const GLclampf blue,
													const GLclampf alpha,
													const GLclampf depth,
													const GLclampf stencil)
	{
		CheckRenderTarget();

		m_CurrentFrameBufferPtr->ClearColorDepthStencilBuffer(red, green, blue, alpha, depth, stencil);
		CheckRenderTarget();
	}

	void RenderSystem::ClearColorDepthBuffer(const GLclampf red,
											 const GLclampf green,
											 const GLclampf blue,
											 const GLclampf alpha,
											 const GLclampf depth)
	{
		CheckRenderTarget();

		m_CurrentFrameBufferPtr->ClearColorDepthBuffer(red, green, blue, alpha, depth);
		CheckRenderTarget();
	}

	void RenderSystem::ClearColorBuffer(const GLclampf red, const GLclampf green, const GLclampf blue, const GLclampf alpha)
	{
		CheckRenderTarget();

		m_CurrentFrameBufferPtr->ClearColorBuffer(red, green, blue, alpha);
		CheckRenderTarget();
	}

	void RenderSystem::ClearDepthStencilBuffer(const GLclampf depth, const GLclampf stencil)
	{
		CheckRenderTarget();

		m_CurrentFrameBufferPtr->ClearDepthStencilBuffer(depth, stencil);
		CheckRenderTarget();
	}

	void RenderSystem::GetDisplayBufferObjectId(const GLenum targetDisplay, GLuint& displayBufferId0, GLuint& displayBufferId1)
	{
		if (!m_InitializeFlag)
		{
			NN_TPANIC_("Initialize() is not called.\n");
		}

		if (targetDisplay == NN_GX_DISPLAY0)
		{
			displayBufferId0 = m_DisplayBuffers0.GetDisplayBufferId(0);
			displayBufferId1 = m_DisplayBuffers0.GetDisplayBufferId(1);
		}
		else if (targetDisplay == NN_GX_DISPLAY1)
		{
			displayBufferId0 = m_DisplayBuffers1.GetDisplayBufferId(0);
			displayBufferId1 = m_DisplayBuffers1.GetDisplayBufferId(1);
		}
		else
		{
			NN_TPANIC_("Invalid display name.\n");
		}
	}

	GLuint RenderSystem::GetFrameBufferObjectId(const GLenum targetDisplay)
	{
		if (!m_InitializeFlag)
		{
			NN_TPANIC_("Initialize() is not called.\n");
		}

		if ((targetDisplay == NN_GX_DISPLAY0) || (targetDisplay == NN_GX_DISPLAY1))
		{
			return m_FrameBuffer0.GetFrameBufferId();
		}
		else
		{
			NN_TPANIC_("Invalid display name.\n");

			return 0;
		}
	}

	GLuint RenderSystem::GetRenderBufferObjectId(const GLenum targetDisplay, const GLenum renderBufferType)
	{
		if (!m_InitializeFlag)
		{
			NN_TPANIC_("Initialize() is not called.\n");
		}

		if ((targetDisplay == NN_GX_DISPLAY0) || (targetDisplay == NN_GX_DISPLAY1))
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
			NN_TPANIC_("Invalid display name.\n");

			return 0;
		}
	}

	void RenderSystem::SwapBuffers(void)
	{
		CheckRenderTarget();

		// Transfer data from the color buffer to the display buffer.
		m_CurrentFrameBufferPtr->Bind();
		m_CurrentDisplayBuffersPtr->Bind();
		// Get the ID of the display buffer data is being transferred to.
		GLuint displayBufferId = m_CurrentDisplayBuffersPtr->GetTargetDisplayBufferId();
		m_CurrentFrameBufferPtr->Transfer(displayBufferId, NN_GX_ANTIALIASE_NOT_USED, GL_FALSE, 0, 0);

		// Execute the command list.
		m_CommandList.Run();
		m_CommandList.Swap();

		nngxSwapBuffers(m_TargetDisplay);

		m_CurrentDisplayBuffersPtr->IncreaseBufferIndex();
	}

	void RenderSystem::WaitVsync(const GLenum targetDisplay)
	{
		CheckRenderTarget();

		if ((targetDisplay == NN_GX_DISPLAY0) || (targetDisplay == NN_GX_DISPLAY1) || (targetDisplay == NN_GX_DISPLAY_BOTH))
		{
			nngxWaitVSync(targetDisplay);
		}
		else
		{
			NN_TPANIC_("Invalid display name.\n");
		}
	}

	void RenderSystem::SetSerialRun(const bool flag) { m_CommandList.SetSerialRun(flag); }

	void RenderSystem::CheckRenderTarget(void)
	{
		if (!m_InitializeFlag)
		{
			NN_TPANIC_("Initialize() is not called.\n");
		}

		if (!((m_TargetDisplay == NN_GX_DISPLAY0) || (m_TargetDisplay == NN_GX_DISPLAY1)))
		{
			NN_TPANIC_("Invalid display name. Call SetRenderTarget().\n");
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
