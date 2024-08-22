#ifndef NP_GRAPHICS_GRAPHICS_DISPLAY_BUFFERS_H_
#define NP_GRAPHICS_GRAPHICS_DISPLAY_BUFFERS_H_

#include <np/graphics/graphics_Utility.h>

namespace np { namespace graphics {
	const u32 GRAPHICS_DISPLAY_BUFFER_NUM = 2;

	/*!
		@brief Stores data for creating multiple display buffers.
	*/
	class DisplayBuffersDescription
	{
	public:
		DisplayBuffersDescription(void);
		virtual ~DisplayBuffersDescription(void);

	public:
		GLenum m_TargetDisplay;
		u32	   m_DisplayBufferNum;
		GLenum m_Format;
		u32	   m_Width;
		u32	   m_Height;
		GLenum m_Area;

	public:
		static DisplayBuffersDescription GetDefaultDisplay0Description(void);
		static DisplayBuffersDescription GetDefaultDisplay1Description(void);
		static DisplayBuffersDescription GetDefaultDisplay0ExtDescription(void);
	};

	/*!
		@brief Represents multiple display buffers.
	*/
	class DisplayBuffers : private nn::util::NonCopyable<DisplayBuffers>
	{
	public:
		DisplayBuffers(void);
		virtual ~DisplayBuffers(void);

	protected:
		void Initialize(const DisplayBuffersDescription& displayBufferDescription);
		void Finalize(void);

	public:
		static void Create(const DisplayBuffersDescription& displayBuffersDesc, DisplayBuffers& displayBuffers);
		static void Destroy(DisplayBuffers& displayBuffers);

	public:
		GLuint GetTargetDisplayBufferId(void) const;
		GLuint GetPreviousTargetDisplayBufferId(void) const;
		GLuint GetDisplayBufferId(const u32 index) const;
		void   Bind(void);
		void   IncreaseBufferIndex(void);

	public:
		u32 GetWidth(void) const;
		u32 GetHeight(void) const;

	protected:
		bool m_InitializeFlag;
		bool m_Padding[3];

		GLenum m_TargetDisplay;
		u32	   m_DisplayBufferNum;
		GLuint m_DisplayBufferIdArray[GRAPHICS_DISPLAY_BUFFER_NUM];
		u32	   m_CurrentBufferIndex;

		u32 m_Width;
		u32 m_Height;
	};
}}	// namespace np::graphics

#endif
