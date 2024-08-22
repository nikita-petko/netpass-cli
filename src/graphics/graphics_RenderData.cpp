#include <np/graphics/graphics_RenderData.h>

namespace np { namespace graphics {

	f32 GetLcdWidth(const s32 display)
	{
		if (display == NN_GX_DISPLAY0)
		{
			return NN_GX_DISPLAY0_WIDTH;
		}
		else if (display == NN_GX_DISPLAY1)
		{
			return NN_GX_DISPLAY1_WIDTH;
		}
		else
		{
			return NN_GX_DISPLAY0_WIDTH;
		}
	}

	f32 GetLcdHeight(const s32 display)
	{
		if (display == NN_GX_DISPLAY0)
		{
			return NN_GX_DISPLAY0_HEIGHT;
		}
		else if (display == NN_GX_DISPLAY1)
		{
			return NN_GX_DISPLAY1_HEIGHT;
		}
		else
		{
			return NN_GX_DISPLAY0_HEIGHT;
		}
	}

	/* ------------------------------------------------------------------------
			 RenderData Class Member Function
	   ------------------------------------------------------------------------ */

	RenderData::RenderData(void)
	: m_RenderTarget(NN_GX_DISPLAY0)
	, m_WindowWidth(NN_GX_DISPLAY0_WIDTH)
	, m_WindowHeight(NN_GX_DISPLAY0_HEIGHT)
	, m_InverseWindowWidth(1.0f / NN_GX_DISPLAY0_WIDTH)
	, m_InverseWindowHeight(1.0f / NN_GX_DISPLAY0_HEIGHT)
	{
		m_WindowWidth  = GetLcdWidth(m_RenderTarget);
		m_WindowHeight = GetLcdHeight(m_RenderTarget);

		CalculateInverseWindowSize();
	}

	RenderData::~RenderData() {}

	void RenderData::Initialize(void) {}

	void RenderData::Finalize(void) {}

	void RenderData::SetWindowSize(const f32 windowWidth, const f32 windowHeight)
	{
		m_WindowWidth  = windowWidth;
		m_WindowHeight = windowHeight;

		CalculateInverseWindowSize();
	}

	void RenderData::GetNormalizedDeviceCoordinateXY(const f32 windowCoordinateX,
													 const f32 windowCoordinateY,
													 f32&	   normalizedDeviceCoordinateX,
													 f32&	   normalizedDeviceCoordinateY)
	{
		normalizedDeviceCoordinateX = GetNormalizedDeviceCoordinateX(m_WindowWidth - windowCoordinateY);
		normalizedDeviceCoordinateY = GetNormalizedDeviceCoordinateY(windowCoordinateX);
	}

	f32 RenderData::GetNormalizedDeviceCoordinateX(const f32 windowCoordinateY)
	{
		return (windowCoordinateY * 2.0f * m_InverseWindowWidth) - 1.0f;
	}

	f32 RenderData::GetNormalizedDeviceCoordinateY(const f32 windowCoordinateX)
	{
		return 1.0f - (windowCoordinateX * 2.0f * m_InverseWindowHeight);
	}

	void RenderData::CalculateInverseWindowSize(void)
	{
		if (m_WindowWidth > 0.0f)
		{
			m_InverseWindowWidth = 1.0f / m_WindowWidth;
		}
		else
		{
			NN_TPANIC_("Invalid windowWidth = %d\n", m_WindowWidth);
		}

		if (m_WindowHeight > 0.0f)
		{
			m_InverseWindowHeight = 1.0f / m_WindowHeight;
		}
		else
		{
			NN_TPANIC_("Invalid windowHeight = %d\n", m_WindowHeight);
		}
	}
}}	// namespace np::graphics
