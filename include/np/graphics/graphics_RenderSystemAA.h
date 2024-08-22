#ifndef NP_GRAPHICS_GRAPHICS_RENDER_SYSTEM_AA_H_
#define NP_GRAPHICS_GRAPHICS_RENDER_SYSTEM_AA_H_

#include <np/graphics/graphics_RenderSystemExt.h>

namespace np { namespace graphics {
	class RenderSystemAA : public RenderSystemExt
	{
	public:
		enum AAMode
		{
			AA_NONE,
			AA_2x1,
			AA_2x2
		};

	public:
		RenderSystemAA(AAMode mode);

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

	public:
		virtual void SetRenderTarget(const GLenum targetDisplay);
		virtual void Transfer(void);

		void SetUnitViewport(f32 Left, f32 Right, f32 Bottom, f32 Top);

	protected:
		f32 m_Left;
		f32 m_Right;
		f32 m_Bottom;
		f32 m_Top;

		AAMode m_AAMode;
		NN_PADDING3;
	};
}}	// namespace np::graphics

#endif
