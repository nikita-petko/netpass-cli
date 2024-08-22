#include <np/graphics/graphics_DisplayBuffers.h>

namespace np { namespace graphics {

	/* ------------------------------------------------------------------------
		DisplayBuffersDescription Class Member Function
		------------------------------------------------------------------------ */

	DisplayBuffersDescription::DisplayBuffersDescription(void)
	: m_TargetDisplay(NN_GX_DISPLAY0)
	, m_DisplayBufferNum(GRAPHICS_DISPLAY_BUFFER_NUM)
	, m_Format(GL_RGB8_OES)
	, m_Width(nn::gx::DISPLAY0_WIDTH)
	, m_Height(nn::gx::DISPLAY0_HEIGHT)
	, m_Area(NN_GX_MEM_FCRAM)
	{
	}

	DisplayBuffersDescription::~DisplayBuffersDescription(void) {}

	DisplayBuffersDescription DisplayBuffersDescription::GetDefaultDisplay0Description(void)
	{
		DisplayBuffersDescription displayBuffersDesc;
		displayBuffersDesc.m_TargetDisplay	  = NN_GX_DISPLAY0;
		displayBuffersDesc.m_DisplayBufferNum = GRAPHICS_DISPLAY_BUFFER_NUM;
		displayBuffersDesc.m_Format			  = GL_RGB8_OES;
		displayBuffersDesc.m_Width			  = nn::gx::DISPLAY0_WIDTH;
		displayBuffersDesc.m_Height			  = nn::gx::DISPLAY0_HEIGHT;
		displayBuffersDesc.m_Area			  = NN_GX_MEM_FCRAM;

		return displayBuffersDesc;
	}

	DisplayBuffersDescription DisplayBuffersDescription::GetDefaultDisplay1Description(void)
	{
		DisplayBuffersDescription displayBuffersDesc;
		displayBuffersDesc.m_TargetDisplay	  = NN_GX_DISPLAY1;
		displayBuffersDesc.m_DisplayBufferNum = GRAPHICS_DISPLAY_BUFFER_NUM;
		displayBuffersDesc.m_Format			  = GL_RGB8_OES;
		displayBuffersDesc.m_Width			  = nn::gx::DISPLAY1_WIDTH;
		displayBuffersDesc.m_Height			  = nn::gx::DISPLAY1_HEIGHT;
		displayBuffersDesc.m_Area			  = NN_GX_MEM_FCRAM;

		return displayBuffersDesc;
	}

	DisplayBuffersDescription DisplayBuffersDescription::GetDefaultDisplay0ExtDescription(void)
	{
		DisplayBuffersDescription displayBuffersDesc;
		displayBuffersDesc.m_TargetDisplay	  = NN_GX_DISPLAY0_EXT;
		displayBuffersDesc.m_Format			  = GL_RGB8_OES;
		displayBuffersDesc.m_Width			  = nn::gx::DISPLAY0_WIDTH;
		displayBuffersDesc.m_Height			  = nn::gx::DISPLAY0_HEIGHT;
		displayBuffersDesc.m_Area			  = NN_GX_MEM_FCRAM;
		displayBuffersDesc.m_DisplayBufferNum = 2;

		return displayBuffersDesc;
	}

	/* ------------------------------------------------------------------------
		  DisplayBuffers Class Member Function
		  ------------------------------------------------------------------------ */

	DisplayBuffers::DisplayBuffers(void)
	: m_InitializeFlag(false)
	, m_TargetDisplay(NN_GX_DISPLAY0)
	, m_DisplayBufferNum(GRAPHICS_DISPLAY_BUFFER_NUM)
	, m_CurrentBufferIndex(0)
	, m_Width(nn::gx::DISPLAY0_WIDTH)
	, m_Height(nn::gx::DISPLAY0_HEIGHT)
	{
	}

	DisplayBuffers::~DisplayBuffers(void) { Finalize(); }

	void DisplayBuffers::Initialize(const DisplayBuffersDescription& displayBufferDesc)
	{
		if (!m_InitializeFlag)
		{
			m_TargetDisplay = displayBufferDesc.m_TargetDisplay;

			m_DisplayBufferNum = displayBufferDesc.m_DisplayBufferNum;

			m_Width	 = displayBufferDesc.m_Width;
			m_Height = displayBufferDesc.m_Height;

			nngxActiveDisplay(displayBufferDesc.m_TargetDisplay);
			nngxGenDisplaybuffers(m_DisplayBufferNum, m_DisplayBufferIdArray);

			for (u32 bufferIndex = 0; bufferIndex < m_DisplayBufferNum; bufferIndex++)
			{
				nngxBindDisplaybuffer(m_DisplayBufferIdArray[bufferIndex]);
				nngxDisplaybufferStorage(displayBufferDesc.m_Format,
										 displayBufferDesc.m_Width,
										 displayBufferDesc.m_Height,
										 displayBufferDesc.m_Area);
				GRAPHICS_ASSERT_GL_ERROR();
			}

			nngxDisplayEnv(0, 0);
			m_InitializeFlag = true;

			GRAPHICS_ASSERT_GL_ERROR();
		}
	}

	void DisplayBuffers::Finalize(void)
	{
		if (m_InitializeFlag)
		{
			nngxBindDisplaybuffer(0);
			GRAPHICS_ASSERT_GL_ERROR();

			nngxDeleteDisplaybuffers(m_DisplayBufferNum, m_DisplayBufferIdArray);
			GRAPHICS_ASSERT_GL_ERROR();

			for (u32 bufferIndex = 0; bufferIndex < m_DisplayBufferNum; bufferIndex++)
			{
				m_DisplayBufferIdArray[bufferIndex] = 0;
			}

			m_InitializeFlag = false;
		}
	}

	void DisplayBuffers::Create(const DisplayBuffersDescription& displayBuffersDesc, DisplayBuffers& displayBuffers)
	{
		displayBuffers.Initialize(displayBuffersDesc);
	}

	void DisplayBuffers::Destroy(DisplayBuffers& displayBuffers) { displayBuffers.Finalize(); }

	GLuint DisplayBuffers::GetTargetDisplayBufferId(void) const { return m_DisplayBufferIdArray[m_CurrentBufferIndex]; }

	GLuint DisplayBuffers::GetPreviousTargetDisplayBufferId(void) const
	{
		if (m_CurrentBufferIndex == 0)
		{
			return m_DisplayBufferIdArray[m_DisplayBufferNum - 1];
		}
		else
		{
			return m_DisplayBufferIdArray[m_CurrentBufferIndex - 1];
		}
	}

	GLuint DisplayBuffers::GetDisplayBufferId(const u32 index) const
	{
		if (index < m_DisplayBufferNum)
		{
			return m_DisplayBufferIdArray[m_CurrentBufferIndex];
		}
		else
		{
			return 0;
		}
	}

	void DisplayBuffers::Bind(void)
	{
		nngxActiveDisplay(m_TargetDisplay);
		nngxBindDisplaybuffer(m_DisplayBufferIdArray[m_CurrentBufferIndex]);
	}

	void DisplayBuffers::IncreaseBufferIndex(void)
	{
		m_CurrentBufferIndex++;
		if (m_CurrentBufferIndex >= m_DisplayBufferNum)
		{
			m_CurrentBufferIndex = 0;
		}
	}

	u32 DisplayBuffers::GetWidth(void) const { return m_Width; }

	u32 DisplayBuffers::GetHeight(void) const { return m_Height; }

}}	// namespace np::graphics
