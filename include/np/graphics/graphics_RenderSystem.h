#ifndef NP_GRAPHICS_GRAPHICS_RENDER_SYSTEM_H_
#define NP_GRAPHICS_GRAPHICS_RENDER_SYSTEM_H_

#include <nn/gx.h>

#include <np/graphics/graphics_MemoryManager.h>
#include <np/graphics/graphics_Utility.h>

#include <np/graphics/graphics_CommandList.h>
#include <np/graphics/graphics_DisplayBuffers.h>
#include <np/graphics/graphics_FrameBuffer.h>

#include <np/graphics/graphics_ShaderManager.h>

namespace np { namespace graphics {
	/*!
		@brief Contains member functions for using display buffer and framebuffer objects on the CTR.
	*/
	class RenderSystem : private nn::util::NonCopyable<RenderSystem>
	{
	public:
		RenderSystem(void);
		virtual ~RenderSystem(void);

	public:
		virtual void Initialize(
			const uptr						 fcramAddress,
			const size_t					 memorySize,
			const u32						 commandBufferSize		= 0x40000,
			const u32						 requestNum				= 128,
			const bool						 serialRunMode			= true,
			const DisplayBuffersDescription& displayBuffers0Desc	= DisplayBuffersDescription::GetDefaultDisplay0Description(),
			const DisplayBuffersDescription& displayBuffers1Desc	= DisplayBuffersDescription::GetDefaultDisplay1Description(),
			const FrameBufferDescription&	 frameBuffer0Desc		= FrameBufferDescription::GetDefaultDisplay0FrameBufferDescription(),
			const DisplayBuffersDescription& displayBuffers0ExtDesc = DisplayBuffersDescription::GetDefaultDisplay0ExtDescription(),
			const bool						 isFillBlackLCD			= true);
		virtual void Finalize(void);

	protected:
		virtual void InitializeLcdDisplay(void);

	public:
		virtual void SetRenderTarget(const GLenum targetDisplay);
		virtual void SetViewport(const GLint x, const GLint y, const GLsizei width, const GLsizei height);

		virtual void Clear(const GLbitfield mask = (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT));
		virtual void ClearColorDepthStencilBuffer(const GLclampf red,
												  const GLclampf green,
												  const GLclampf blue,
												  const GLclampf alpha,
												  const GLclampf depth	 = 1.0f,
												  const GLclampf stencil = 0.0f);
		virtual void ClearColorDepthBuffer(const GLclampf red,
										   const GLclampf green,
										   const GLclampf blue,
										   const GLclampf alpha,
										   const GLclampf depth = 1.0f);
		virtual void ClearColorBuffer(const GLclampf red, const GLclampf green, const GLclampf blue, const GLclampf alpha);
		virtual void ClearDepthStencilBuffer(const GLclampf depth = 1.0f, const GLclampf stencil = 0.0f);

	public:
		virtual void   GetDisplayBufferObjectId(const GLenum targetDisplay, GLuint& DisplayBufferId0, GLuint& DisplayBufferId1);
		virtual GLuint GetFrameBufferObjectId(const GLenum targetDisplay);
		virtual GLuint GetRenderBufferObjectId(const GLenum targetDisplay, const GLenum renderBufferType);

	public:
		virtual void SwapBuffers(void);
		virtual void WaitVsync(const GLenum targetDisplay);
		virtual void SetSerialRun(const bool flag);
		virtual void CheckRenderTarget(void);

	public:
		bool m_InitializeFlag;
		bool m_InitializeLcdDisplayFlag;
		bool m_Padding[2];

		GLenum m_TargetDisplay;

		DisplayBuffers* m_CurrentDisplayBuffersPtr;
		FrameBuffer*	m_CurrentFrameBufferPtr;

		DisplayBuffers m_DisplayBuffers0;
		FrameBuffer	   m_FrameBuffer0;

		DisplayBuffers m_DisplayBuffers1;

		CommandList m_CommandList;
	};
}}	// namespace np::graphics

#endif
