#include <np/graphics/graphics_TextsRenderData.h>

namespace np { namespace graphics {

	/* ------------------------------------------------------------------------
			 TextsRenderData Class Member Function
	   ------------------------------------------------------------------------ */

	TextsRenderData::TextsRenderData(void)
	: TrianglesRenderData()
	, m_PackedTextLength(0)
	, m_MaxTextLength(MAX_TEXT_LENGTH)
	, m_FontSize(8.0f)
	, m_FontBaseSize(FONT_BASE_SIZE)
	, m_FontOffsetS(FONT_OFFSET_S)
	, m_FontOffsetT(FONT_OFFSET_T)
	{
		m_Color[0] = 1.0f;
		m_Color[1] = 1.0f;
		m_Color[2] = 1.0f;
		m_Color[3] = 1.0f;
	}

	TextsRenderData::~TextsRenderData(void) { Finalize(); }

	void TextsRenderData::Initialize(void) {}

	void TextsRenderData::Finalize(void)
	{
		ClearPackedTextLength();
		TrianglesRenderData::Finalize();
	}

	void TextsRenderData::InitializeVertexBuffers(const u32 vertexAttributes, const GLenum triangleType, const u32 maxTextLength)
	{
		SetVertexAttributes(vertexAttributes);
		SetTriangleType(triangleType);

		ClearPackedTextLength();
		SetMaxTextLength(maxTextLength);
	}

	void TextsRenderData::SetMaxTextLength(const u32 maxTextLength)
	{
		m_PackedTextLength = 0;
		m_MaxTextLength	   = maxTextLength;

		u32 verticesNum	 = 4 * m_MaxTextLength;
		u32 trianglesNum = 2 * m_MaxTextLength;
		SetVerticesNum(verticesNum);
		SetTrianglesNum(trianglesNum);

		CreateArrays();
		SetPackedTextLength(0);
	}

	void TextsRenderData::AddText(const f32	  windowCoordinateX,
								  const f32	  windowCoordinateY,
								  const char* text,
								  const u32	  textLength,
								  const f32	  normalizedDeviceCoordinateZ)
	{
		if ((m_PackedTextLength + textLength) >= m_MaxTextLength)
		{
			return;
		}

		for (u32 i = 0; i < textLength; ++i)
		{
			u32 index4 = 4 * (i + m_PackedTextLength);
			u32 index2 = 2 * (i + m_PackedTextLength);

			s32 fontW  = text[i] - 0x20;
			f32 fontS  = static_cast<f32>(fontW % m_FontBaseSize) * m_FontOffsetS;
			f32 fontT  = static_cast<f32>(fontW / m_FontBaseSize) * m_FontOffsetT;
			f32 offset = static_cast<f32>(i) * m_FontSize;

			// POSITION
			f32 normalizedDeviceCoordinateX = 0.0f;
			f32 normalizedDeviceCoordinateY = 0.0f;
			GetNormalizedDeviceCoordinateXY(windowCoordinateX + offset,
											windowCoordinateY + m_FontSize,
											normalizedDeviceCoordinateX,
											normalizedDeviceCoordinateY);
			SetPosition(index4, normalizedDeviceCoordinateX, normalizedDeviceCoordinateY, normalizedDeviceCoordinateZ, 1.0f);

			GetNormalizedDeviceCoordinateXY(windowCoordinateX + offset,
											windowCoordinateY,
											normalizedDeviceCoordinateX,
											normalizedDeviceCoordinateY);
			SetPosition(index4 + 1, normalizedDeviceCoordinateX, normalizedDeviceCoordinateY, normalizedDeviceCoordinateZ, 1.0f);

			GetNormalizedDeviceCoordinateXY(windowCoordinateX + offset + m_FontSize,
											windowCoordinateY + m_FontSize,
											normalizedDeviceCoordinateX,
											normalizedDeviceCoordinateY);
			SetPosition(index4 + 2, normalizedDeviceCoordinateX, normalizedDeviceCoordinateY, normalizedDeviceCoordinateZ, 1.0f);

			GetNormalizedDeviceCoordinateXY(windowCoordinateX + offset + m_FontSize,
											windowCoordinateY,
											normalizedDeviceCoordinateX,
											normalizedDeviceCoordinateY);
			SetPosition(index4 + 3, normalizedDeviceCoordinateX, normalizedDeviceCoordinateY, normalizedDeviceCoordinateZ, 1.0f);

			// COLOR
			for (u32 j = 0; j < 4; j++)
			{
				SetColor(index4 + j, m_Color[0], m_Color[1], m_Color[2], m_Color[3]);
			}

			// TEXCOORD
			f32 s = fontS + m_FontOffsetS;
			f32 t = fontT + m_FontOffsetT;
			SetTexcoord(index4, s, t, 0.0f);

			s = fontS + m_FontOffsetS;
			t = fontT;
			SetTexcoord(index4 + 1, s, t, 0.0f);

			s = fontS;
			t = fontT + m_FontOffsetT;
			SetTexcoord(index4 + 2, s, t, 0.0f);

			s = fontS;
			t = fontT;
			SetTexcoord(index4 + 3, s, t, 0.0f);

			// INDEX
			SetIndex(index2, index4, index4 + 1, index4 + 2);

			SetIndex(index2 + 1, index4 + 2, index4 + 1, index4 + 3);
		}

		m_PackedTextLength += textLength;
		SetPackedTextLength(m_PackedTextLength);
	}

	void TextsRenderData::ClearPackedTextLength(void) { SetPackedTextLength(0); }

	void TextsRenderData::SetPackedTextLength(const u32 packedTextLength)
	{
		m_PackedTextLength = packedTextLength;
		SetPackedVerticesNum(m_PackedTextLength * 4);
		SetPackedTrianglesNum(m_PackedTextLength * 2);
	}

	u32 TextsRenderData::GetPackedTextLength(void) const { return m_PackedTextLength; }

	void TextsRenderData::SetFontSize(const f32 fontSize) { m_FontSize = fontSize; }

	void TextsRenderData::SetFontColor(const f32 red, const f32 green, const f32 blue, const f32 alpha)
	{
		m_Color[0] = red;
		m_Color[1] = green;
		m_Color[2] = blue;
		m_Color[3] = alpha;
	}

}}	// namespace np::graphics
