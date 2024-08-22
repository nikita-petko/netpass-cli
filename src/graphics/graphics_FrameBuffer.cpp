#include <np/graphics/graphics_FrameBuffer.h>

namespace np { namespace graphics {

	/* ------------------------------------------------------------------------
		  RenderBuffersDescription Class Member Function
		  ------------------------------------------------------------------------ */

	RenderBufferDescription::RenderBufferDescription(void)
	: m_Format(GL_RGBA8_OES)
	, m_Width(nn::gx::DISPLAY0_WIDTH)
	, m_Height(nn::gx::DISPLAY0_HEIGHT)
	, m_Area(NN_GX_MEM_FCRAM)
	, m_Attachment(GL_COLOR_ATTACHMENT0)
	{
	}

	RenderBufferDescription::~RenderBufferDescription(void) {}

	RenderBufferDescription RenderBufferDescription::GetDefaultDisplay0ColorDescription(void)
	{
		RenderBufferDescription renderBufferDesc;
		renderBufferDesc.m_Format	  = GL_RGBA8_OES;
		renderBufferDesc.m_Width	  = nn::gx::DISPLAY0_WIDTH;
		renderBufferDesc.m_Height	  = nn::gx::DISPLAY0_HEIGHT;
		renderBufferDesc.m_Area		  = NN_GX_MEM_VRAMA;
		renderBufferDesc.m_Attachment = GL_COLOR_ATTACHMENT0;

		return renderBufferDesc;
	}

	RenderBufferDescription RenderBufferDescription::GetDefaultDisplay0DepthStencilDescription(void)
	{
		RenderBufferDescription renderBufferDesc;
		renderBufferDesc.m_Format	  = GL_DEPTH24_STENCIL8_EXT;
		renderBufferDesc.m_Width	  = nn::gx::DISPLAY0_WIDTH;
		renderBufferDesc.m_Height	  = nn::gx::DISPLAY0_HEIGHT;
		renderBufferDesc.m_Area		  = NN_GX_MEM_VRAMB;
		renderBufferDesc.m_Attachment = GL_DEPTH_STENCIL_ATTACHMENT;

		return renderBufferDesc;
	}

	RenderBufferDescription RenderBufferDescription::GetDefaultDisplay1ColorDescription(void)
	{
		RenderBufferDescription renderBufferDesc;
		renderBufferDesc.m_Format	  = GL_RGBA8_OES;
		renderBufferDesc.m_Width	  = nn::gx::DISPLAY1_WIDTH;
		renderBufferDesc.m_Height	  = nn::gx::DISPLAY1_HEIGHT;
		renderBufferDesc.m_Area		  = NN_GX_MEM_VRAMA;
		renderBufferDesc.m_Attachment = GL_COLOR_ATTACHMENT0;

		return renderBufferDesc;
	}

	RenderBufferDescription RenderBufferDescription::GetDefaultDisplay1DepthStencilDescription(void)
	{
		RenderBufferDescription renderBufferDesc;
		renderBufferDesc.m_Format	  = GL_DEPTH24_STENCIL8_EXT;
		renderBufferDesc.m_Width	  = nn::gx::DISPLAY1_WIDTH;
		renderBufferDesc.m_Height	  = nn::gx::DISPLAY1_HEIGHT;
		renderBufferDesc.m_Area		  = NN_GX_MEM_VRAMB;
		renderBufferDesc.m_Attachment = GL_DEPTH_STENCIL_ATTACHMENT;

		return renderBufferDesc;
	}

	/* ------------------------------------------------------------------------
		  FrameBuffersDescription Class Member Function
		  ------------------------------------------------------------------------ */

	FrameBufferDescription::FrameBufferDescription(void) { Initialize(); }

	FrameBufferDescription::~FrameBufferDescription(void) { Finalize(); }

	FrameBufferDescription::FrameBufferDescription(const FrameBufferDescription& rhs)
	{
		Initialize();

		for (u32 index = 0; index < GRAPHICS_RENDER_BUFFER_NUM; index++)
		{
			m_RenderBufferDescriptionArray[index] = rhs.m_RenderBufferDescriptionArray[index];
		}
	}

	FrameBufferDescription& FrameBufferDescription::operator=(const FrameBufferDescription& rhs)
	{
		Finalize();
		Initialize();

		for (u32 index = 0; index < GRAPHICS_RENDER_BUFFER_NUM; index++)
		{
			m_RenderBufferDescriptionArray[index] = rhs.m_RenderBufferDescriptionArray[index];
		}

		return (*this);
	}

	void FrameBufferDescription::Initialize(void) {}

	void FrameBufferDescription::Finalize(void) {}

	FrameBufferDescription FrameBufferDescription::GetDefaultDisplay0FrameBufferDescription(void)
	{
		FrameBufferDescription frameBufferDesc;
		frameBufferDesc.m_RenderBufferDescriptionArray[GRAPHICS_COLOR_BUFFER_INDEX] =
			RenderBufferDescription::GetDefaultDisplay0ColorDescription();
		frameBufferDesc.m_RenderBufferDescriptionArray[GRAPHICS_DEPTH_STENCIL_BUFFER_INDEX] =
			RenderBufferDescription::GetDefaultDisplay0DepthStencilDescription();

		return frameBufferDesc;
	}

	FrameBufferDescription FrameBufferDescription::GetDefaultDisplay1FrameBufferDescription(void)
	{
		FrameBufferDescription frameBufferDesc;
		frameBufferDesc.m_RenderBufferDescriptionArray[GRAPHICS_COLOR_BUFFER_INDEX] =
			RenderBufferDescription::GetDefaultDisplay1ColorDescription();
		frameBufferDesc.m_RenderBufferDescriptionArray[GRAPHICS_DEPTH_STENCIL_BUFFER_INDEX] =
			RenderBufferDescription::GetDefaultDisplay1DepthStencilDescription();

		return frameBufferDesc;
	}

	/* ------------------------------------------------------------------------
		  FrameBuffer Class Member Function
		  ------------------------------------------------------------------------ */

	FrameBuffer::FrameBuffer(void)
	: m_InitializeFlag(false)
	, m_FrameBufferId(0)
	, m_RenderBufferNum(GRAPHICS_RENDER_BUFFER_NUM)
	, m_Width(nn::gx::DISPLAY0_WIDTH)
	, m_Height(nn::gx::DISPLAY0_HEIGHT)
	{
	}

	FrameBuffer::~FrameBuffer(void) { Finalize(); }

	void FrameBuffer::Initialize(const FrameBufferDescription& frameBufferDesc)
	{
		if (!m_InitializeFlag)
		{
			GRAPHICS_ASSERT_GL_ERROR();

			glGenFramebuffers(1, &m_FrameBufferId);
			GRAPHICS_ASSERT_GL_ERROR();

			glGenRenderbuffers(m_RenderBufferNum, m_RenderBufferIdArray);
			GRAPHICS_ASSERT_GL_ERROR();

			glBindFramebuffer(GL_FRAMEBUFFER, m_FrameBufferId);
			GRAPHICS_ASSERT_GL_ERROR();

			bool colorBufferAttached		= false;
			bool depthStencilBufferAttached = false;
			bool depthBufferAttached		= false;
			bool stencilBufferAttached		= false;

			for (u32 bufferIndex = 0; bufferIndex < GRAPHICS_RENDER_BUFFER_NUM; bufferIndex++)
			{
				const RenderBufferDescription& bufferDesc = frameBufferDesc.m_RenderBufferDescriptionArray[bufferIndex];
				GLuint&						   bufferId	  = m_RenderBufferIdArray[bufferIndex];

				glBindRenderbuffer(GL_RENDERBUFFER, bufferId);
				glRenderbufferStorage(GL_RENDERBUFFER | bufferDesc.m_Area, bufferDesc.m_Format, bufferDesc.m_Width, bufferDesc.m_Height);
				m_Width	 = bufferDesc.m_Width;
				m_Height = bufferDesc.m_Height;
				GRAPHICS_ASSERT_GL_ERROR();

				glFramebufferRenderbuffer(GL_FRAMEBUFFER, bufferDesc.m_Attachment, GL_RENDERBUFFER, m_RenderBufferIdArray[bufferIndex]);
				GRAPHICS_ASSERT_GL_ERROR();

				if (bufferDesc.m_Attachment == GL_COLOR_ATTACHMENT0)
				{
					if (colorBufferAttached)
					{
						NN_TPANIC_("Color buffer is attached twice.\n");
					}
					else
					{
						colorBufferAttached = true;
					}
				}

				if (bufferDesc.m_Attachment == GL_DEPTH_STENCIL_ATTACHMENT)
				{
					if (depthStencilBufferAttached)
					{
						NN_TPANIC_("Depth-Stencil buffer is attached twice.\n");
					}
					else
					{
						depthStencilBufferAttached = true;
					}
				}

				if (bufferDesc.m_Attachment == GL_DEPTH_ATTACHMENT)
				{
					if (depthBufferAttached)
					{
						NN_TPANIC_("Depth buffer is attached twice.\n");
					}
					else
					{
						depthBufferAttached = true;
					}
				}

				if (bufferDesc.m_Attachment == GL_STENCIL_ATTACHMENT)
				{
					if (stencilBufferAttached)
					{
						NN_TPANIC_("Stencil buffer is attached twice.\n");
					}
					else
					{
						stencilBufferAttached = true;
					}
				}
			}

			m_InitializeFlag = true;
		}
	}

	void FrameBuffer::Finalize(void)
	{
		if (m_InitializeFlag)
		{
			GRAPHICS_ASSERT_GL_ERROR();

			glBindFramebuffer(GL_FRAMEBUFFER, 0);
			glDeleteFramebuffers(1, &m_FrameBufferId);
			GRAPHICS_ASSERT_GL_ERROR();
			m_FrameBufferId = 0;

			glDeleteRenderbuffers(m_RenderBufferNum, m_RenderBufferIdArray);
			GRAPHICS_ASSERT_GL_ERROR();

			m_InitializeFlag = false;
		}
	}

	void FrameBuffer::Create(const FrameBufferDescription& frameBufferDesc, FrameBuffer& frameBuffer)
	{
		frameBuffer.Initialize(frameBufferDesc);
		GRAPHICS_ASSERT_GL_ERROR();
	}

	void FrameBuffer::Destroy(FrameBuffer& frameBuffer)
	{
		frameBuffer.Finalize();
		GRAPHICS_ASSERT_GL_ERROR();
	}

	void FrameBuffer::Bind(void)
	{
		if (!this->m_InitializeFlag)
		{
			NN_TPANIC_("Not initialized.\n");
		}

		glBindFramebuffer(GL_FRAMEBUFFER, m_FrameBufferId);
	}

	void FrameBuffer::SetViewport(void) { SetViewport(0, 0, GetWidth(), GetHeight()); }

	void FrameBuffer::SetViewport(const GLint x, const GLint y, const GLsizei width, const GLsizei height)
	{
		if (!this->m_InitializeFlag)
		{
			NN_TPANIC_("Not initialized.\n");
		}

		glViewport(x, y, width, height);
	}

	GLsizei FrameBuffer::GetWidth(void) { return m_Width; }

	GLsizei FrameBuffer::GetHeight(void) { return m_Height; }

	void FrameBuffer::ClearBuffer(const GLbitfield mask)
	{
		Bind();
		glClear(mask);
	}

	void FrameBuffer::ClearColorDepthStencilBuffer(const GLclampf red,
												   const GLclampf green,
												   const GLclampf blue,
												   const GLclampf alpha,
												   const GLclampf depth,
												   const GLclampf stencil)
	{
		Bind();
		glClearColor(red, green, blue, alpha);
		glClearDepthf(depth);
		glClearStencil(stencil);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	}

	void FrameBuffer::ClearColorDepthBuffer(const GLclampf red,
											const GLclampf green,
											const GLclampf blue,
											const GLclampf alpha,
											const GLclampf depth)
	{
		Bind();
		glClearColor(red, green, blue, alpha);
		glClearDepthf(depth);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}

	void FrameBuffer::ClearColorBuffer(const GLclampf red, const GLclampf green, const GLclampf blue, const GLclampf alpha)
	{
		Bind();
		glClearColor(red, green, blue, alpha);
		glClear(GL_COLOR_BUFFER_BIT);
	}

	void FrameBuffer::ClearDepthStencilBuffer(const GLclampf depth, const GLclampf stencil)
	{
		Bind();
		glClearDepthf(depth);
		glClearStencil(stencil);
		glClear(GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	}

	void FrameBuffer::ClearDepthBuffer(const GLclampf depth)
	{
		Bind();
		glClearDepthf(depth);
		glClear(GL_DEPTH_BUFFER_BIT);
	}

	void FrameBuffer::ClearStencilBuffer(const GLclampf stencil)
	{
		Bind();
		glClearStencil(stencil);
		glClear(GL_STENCIL_BUFFER_BIT);
	}

	void FrameBuffer::Transfer(const GLuint	   displayBufferId,
							   const GLenum	   mode,
							   const GLboolean yflip,
							   const GLint	   colorX,
							   const GLint	   colorY)
	{
		nngxTransferRenderImage(displayBufferId, mode, yflip, colorX, colorY);
		GRAPHICS_ASSERT_GL_ERROR();
	}

	GLuint FrameBuffer::GetFrameBufferId(void)
	{
		if (!this->m_InitializeFlag)
		{
			NN_TPANIC_("Not initialized.\n");
		}

		return m_FrameBufferId;
	}

	GLuint FrameBuffer::GetRenderBufferId(const u32 bufferIndex)
	{
		if (!this->m_InitializeFlag)
		{
			NN_TPANIC_("Not initialized.\n");

			return 0;
		}

		if (bufferIndex < m_RenderBufferNum)
		{
			return m_RenderBufferIdArray[bufferIndex];
		}
		else
		{
			NN_TPANIC_("Invalid bufferIndex = %d.\n", bufferIndex);

			return 0;
		}
	}

}}	// namespace np::graphics
