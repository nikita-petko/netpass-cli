#include <np/graphics/graphics_RenderSystemAA.h>

namespace np { namespace graphics {
	RenderSystemAA::RenderSystemAA(AAMode mode) : RenderSystemExt(), m_Left(-1.f), m_Right(1.f), m_Bottom(-1.f), m_Top(1.f), m_AAMode(mode)
	{
	}

	void RenderSystemAA::Initialize(const uptr						 fcramAddress,
									const size_t					 memorySize,
									const u32						 commandBufferSize,
									const u32						 requestNum,
									const bool						 serialRunMode,
									const DisplayBuffersDescription& displayBuffers0Desc,
									const DisplayBuffersDescription& displayBuffers1Desc,
									const FrameBufferDescription&	 frameBuffer0Desc,
									const DisplayBuffersDescription& displayBuffers0ExtDesc,
									const bool						 isFillBlackLCD)
	{
		FrameBufferDescription frameBufferAADesc(frameBuffer0Desc);

		if (m_AAMode == AA_2x1)
		{
			for (size_t i = 0; i < GRAPHICS_RENDER_BUFFER_NUM; i++)
			{
				frameBufferAADesc.m_RenderBufferDescriptionArray[i].m_Width *= 2.f;
			}
		}
		if (m_AAMode == AA_2x2)
		{
			for (size_t i = 0; i < GRAPHICS_RENDER_BUFFER_NUM; i++)
			{
				frameBufferAADesc.m_RenderBufferDescriptionArray[i].m_Height *= 2.f;
				frameBufferAADesc.m_RenderBufferDescriptionArray[i].m_Width *= 2.f;
			}
		}
		RenderSystemExt::Initialize(fcramAddress,
									memorySize,
									commandBufferSize,
									requestNum,
									serialRunMode,
									displayBuffers0Desc,
									displayBuffers1Desc,
									frameBufferAADesc,
									displayBuffers0ExtDesc,
									isFillBlackLCD);
	}

	void RenderSystemAA::SetRenderTarget(const GLenum targetDisplay)
	{
		RenderSystemExt::SetRenderTarget(targetDisplay);

		m_CurrentFrameBufferPtr->Bind();
		int k1 = 1, k2 = 1;
		if (targetDisplay == NN_GX_DISPLAY0 || targetDisplay == NN_GX_DISPLAY0_EXT)
		{
			if (m_AAMode == AA_2x1)
			{
				k1 = 2;
			}
			if (m_AAMode == AA_2x2)
			{
				k1 = 2;
				k2 = 2;
			}
		}

		if (targetDisplay != NN_GX_DISPLAY1)
		{
			int pixelWidth	= k2 * m_CurrentDisplayBuffersPtr->GetHeight();
			int pixelHeight = k1 * m_CurrentDisplayBuffersPtr->GetWidth();

			GLint x = GLint((-0.5f * m_Right + 0.5f) * pixelWidth);
			GLint y = GLint((0.5f * m_Bottom + 0.5f) * pixelHeight);
			GLint w = GLint(0.5f * (m_Right - m_Left) * pixelWidth);
			GLint h = GLint(0.5f * (m_Top - m_Bottom) * pixelHeight);

			SetViewport(y, x, h, w);
		}
		else
		{
			SetViewport(0, 0, k1 * m_CurrentDisplayBuffersPtr->GetWidth(), k2 * m_CurrentDisplayBuffersPtr->GetHeight());
		}
	}
	// In stereo display mode, after the image is transferred to both display buffers in the upper LCD, you must call
	// nngxSwapBuffers(NN_GX_DISPLAY0). Call the Transfer function to send the images to the display buffers.
	//
	//
	//
	void RenderSystemAA::Transfer(void)
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
		GLenum mode = NN_GX_ANTIALIASE_NOT_USED;
		if (m_TargetDisplay == NN_GX_DISPLAY0 || m_TargetDisplay == NN_GX_DISPLAY0_EXT)
		{
			switch (m_AAMode)
			{
				case AA_NONE:
					mode = NN_GX_ANTIALIASE_NOT_USED;
					break;
				case AA_2x1:
					mode = NN_GX_ANTIALIASE_2x1;
					break;
				case AA_2x2:
					mode = NN_GX_ANTIALIASE_2x2;
					break;
			}
		}
		else
		{
			mode = NN_GX_ANTIALIASE_NOT_USED;
		}
		m_CurrentDisplayBuffersPtr->Bind();
		m_CurrentFrameBufferPtr->Transfer(displayBufferId, mode, GL_FALSE, 0, 0);
		GRAPHICS_ASSERT_GL_ERROR();

		// Execute the command list.
		m_CommandList.Run();
		m_CommandList.Swap();

		GRAPHICS_ASSERT_GL_ERROR();

		m_CurrentDisplayBuffersPtr->IncreaseBufferIndex();
	}

	void RenderSystemAA::SetUnitViewport(f32 Left, f32 Right, f32 Bottom, f32 Top)
	{
		m_Left	 = Left;
		m_Right	 = Right;
		m_Bottom = Bottom;
		m_Top	 = Top;
	}

}}	// namespace np::graphics
