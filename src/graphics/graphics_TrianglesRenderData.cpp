#include <np/graphics/graphics_TrianglesRenderData.h>

namespace np { namespace graphics {

	/* ------------------------------------------------------------------------
		TrianglesRenderData Class Member Function
	------------------------------------------------------------------------ */

	TrianglesRenderData::TrianglesRenderData(void)
	: RenderData()
	, m_VertexAttributes(VERTEX_NONE_ATTRIBUTE)
	, m_TriangleType(GL_TRIANGLES)
	, m_VerticesNum(0)
	, m_TrianglesNum(0)
	, m_PackedVerticesNum(0)
	, m_PackedTrianglesNum(0)
	, m_PositionArraySize(0)
	, m_ColorArraySize(0)
	, m_TexcoordArraySize(0)
	, m_NormalArraySize(0)
	, m_TangentArraySize(0)
	, m_TotalArraySize(0)
	, m_OldTotalArraySize(0)
	, m_IndexArraySize(0)
	, m_OldIndexArraySize(0)
	, m_PositionArray(NULL)
	, m_ColorArray(NULL)
	, m_TexcoordArray(NULL)
	, m_NormalArray(NULL)
	, m_TangentArray(NULL)
	, m_IndexArray(NULL)
	, m_ArrayBufferId(0)
	, m_ElementArrayBufferId(0)
	, m_InitArrayFlag(false)
	, m_InitBufferFlag(false)
	, m_UpdateBufferFlag(true)
	, m_UpdateAttributesFlag(true)
	, m_PositionX(0.0f)
	, m_PositionY(0.0f)
	, m_PositionZ(0.0f)
	, m_AngleX(0.0f)
	, m_AngleY(0.0f)
	, m_AngleZ(0.0f)
	{
	}

	TrianglesRenderData::~TrianglesRenderData(void) { Finalize(); }

	void TrianglesRenderData::Initialize(void) {}

	void TrianglesRenderData::Finalize(void)
	{
		DestroyArrays();
		DestroyBuffers();
	}

	void TrianglesRenderData::InitializeVertexBuffers(const u32	   vertexAttributes,
													  const GLenum triangleType,
													  const u32	   verticesNum,
													  const u32	   trianglesNum)
	{
		SetVertexAttributes(vertexAttributes);
		SetTriangleType(triangleType);
		SetVerticesNum(verticesNum);
		SetTrianglesNum(trianglesNum);

		CreateArrays();
		SetPackedVerticesNum(0);
		SetPackedTrianglesNum(0);
		SetPackedArraysSize();
	}

	void TrianglesRenderData::SetVertexAttributes(const u32 vertexAttributes) { m_VertexAttributes = vertexAttributes; }

	void TrianglesRenderData::SetTriangleType(const GLenum triangleType) { m_TriangleType = triangleType; }

	void TrianglesRenderData::SetVerticesNum(const u32 verticesNum) { m_VerticesNum = verticesNum; }

	void TrianglesRenderData::SetTrianglesNum(const u32 trianglesNum) { m_TrianglesNum = trianglesNum; }

	void TrianglesRenderData::CreateArrays(void)
	{
		DestroyArrays();

		m_TotalArraySize	= 0;
		m_OldTotalArraySize = 0;

		if (m_VertexAttributes & VERTEX_POSITION_ATTRIBUTE)
		{
			m_PositionArray = (f32*)graphics::Alloc(m_VerticesNum * 4 * 4);
			if (m_PositionArray == NULL)
			{
				NN_TPANIC_("m_PositionArray is NULL.\n");
			}

			for (u32 i = 0; i < m_VerticesNum; i++)
			{
				m_PositionArray[4 * i]	   = 0.0f;
				m_PositionArray[4 * i + 1] = 0.0f;
				m_PositionArray[4 * i + 2] = 0.0f;
				m_PositionArray[4 * i + 3] = 1.0f;
			}
		}

		if (m_VertexAttributes & VERTEX_COLOR_ATTRIBUTE)
		{
			m_ColorArray = (f32*)graphics::Alloc(m_VerticesNum * 4 * 4);
			if (m_ColorArray == NULL)
			{
				NN_TPANIC_("m_ColorArray is NULL.\n");
			}

			for (u32 i = 0; i < m_VerticesNum; i++)
			{
				m_ColorArray[4 * i]		= 0.0f;
				m_ColorArray[4 * i + 1] = 0.0f;
				m_ColorArray[4 * i + 2] = 0.0f;
				m_ColorArray[4 * i + 3] = 1.0f;
			}
		}

		if (m_VertexAttributes & VERTEX_TEXCOORD_ATTRIBUTE)
		{
			m_TexcoordArray = (f32*)graphics::Alloc(m_VerticesNum * 3 * 4);
			if (m_TexcoordArray == NULL)
			{
				NN_TPANIC_("m_TexcoordArray is NULL.\n");
			}

			for (u32 i = 0; i < m_VerticesNum; i++)
			{
				m_TexcoordArray[3 * i]	   = 0.0f;
				m_TexcoordArray[3 * i + 1] = 0.0f;
				m_TexcoordArray[3 * i + 2] = 0.0f;
			}
		}

		if (m_VertexAttributes & VERTEX_NORMAL_ATTRIBUTE)
		{
			m_NormalArray = (f32*)graphics::Alloc(m_VerticesNum * 3 * 4);
			if (m_NormalArray == NULL)
			{
				NN_TPANIC_("m_NormalArray is NULL.\n");
			}

			for (u32 i = 0; i < m_VerticesNum; i++)
			{
				m_NormalArray[3 * i]	 = 0.0f;
				m_NormalArray[3 * i + 1] = 0.0f;
				m_NormalArray[3 * i + 2] = 1.0f;
			}
		}

		if (m_VertexAttributes & VERTEX_TANGENT_ATTRIBUTE)
		{
			m_TangentArray = (f32*)graphics::Alloc(m_VerticesNum * 3 * 4);
			if (m_TangentArray == NULL)
			{
				NN_TPANIC_("m_TangentArray is NULL.\n");
			}

			for (u32 i = 0; i < m_VerticesNum; i++)
			{
				m_TangentArray[3 * i]	  = 1.0f;
				m_TangentArray[3 * i + 1] = 0.0f;
				m_TangentArray[3 * i + 2] = 0.0f;
			}
		}

		m_IndexArraySize	= 0;
		m_OldIndexArraySize = 0;

		m_IndexArray = (GLushort*)graphics::Alloc(m_TrianglesNum * 3 * 2);
		if (m_IndexArray == NULL)
		{
			NN_TPANIC_("m_IndexArray is NULL.\n");
		}

		for (u32 i = 0; i < m_TrianglesNum; i++)
		{
			m_IndexArray[3 * i]		= 0;
			m_IndexArray[3 * i + 1] = 0;
			m_IndexArray[3 * i + 2] = 0;
		}

		m_InitArrayFlag = true;
	}

	void TrianglesRenderData::DestroyArrays(void)
	{
		if (!m_InitArrayFlag)
		{
			return;
		}

		m_InitArrayFlag = false;

		if (m_PositionArray != NULL)
		{
			graphics::Free((void*)m_PositionArray);
			m_PositionArray = NULL;
		}

		if (m_ColorArray != NULL)
		{
			graphics::Free((void*)m_ColorArray);
			m_ColorArray = NULL;
		}

		if (m_TexcoordArray != NULL)
		{
			graphics::Free((void*)m_TexcoordArray);
			m_TexcoordArray = NULL;
		}

		if (m_NormalArray != NULL)
		{
			graphics::Free((void*)m_NormalArray);
			m_NormalArray = NULL;
		}

		if (m_TangentArray != NULL)
		{
			graphics::Free((void*)m_TangentArray);
			m_TangentArray = NULL;
		}

		if (m_IndexArray != NULL)
		{
			graphics::Free((void*)m_IndexArray);
			m_IndexArray = NULL;
		}

		m_PositionArraySize = 0;
		m_NormalArraySize	= 0;
		m_TexcoordArraySize = 0;
		m_ColorArraySize	= 0;
		m_TangentArraySize	= 0;
		m_TotalArraySize	= 0;
		m_OldTotalArraySize = 0;

		m_IndexArraySize	= 0;
		m_OldIndexArraySize = 0;
	}

	void TrianglesRenderData::SetPackedVerticesNum(const u32 packedVerticesNum) { m_PackedVerticesNum = packedVerticesNum; }

	void TrianglesRenderData::AddPackedVerticesNum(const u32 packedVerticesNum) { m_PackedVerticesNum += packedVerticesNum; }

	u32 TrianglesRenderData::GetPackedVerticesNum(void) const { return m_PackedVerticesNum; }

	void TrianglesRenderData::SetPackedTrianglesNum(const u32 packedTrianglesNum) { m_PackedTrianglesNum = packedTrianglesNum; }

	void TrianglesRenderData::AddPackedTrianglesNum(const u32 packedTrianglesNum) { m_PackedTrianglesNum += packedTrianglesNum; }

	u32 TrianglesRenderData::GetPackedTrianglesNum(void) const { return m_PackedTrianglesNum; }

	void TrianglesRenderData::DrawPackedTriangles(void)
	{
		SetPackedArraysSize();
		u32 packedTrianglesNum = GetPackedTrianglesNum();
		if (packedTrianglesNum != 0)
		{
			Draw();
		}
		ClearPackedNum();
	}

	void TrianglesRenderData::ClearPackedNum(void)
	{
		SetPackedVerticesNum(0);
		SetPackedTrianglesNum(0);
	}

	void TrianglesRenderData::SetPackedArraysSize(void)
	{
		m_OldTotalArraySize = m_TotalArraySize;
		m_TotalArraySize	= 0;

		if (m_VertexAttributes & VERTEX_POSITION_ATTRIBUTE)
		{
			m_PositionArraySize = m_PackedVerticesNum * VERTEX_POSITION_ATTRIBUTE_SIZE * 4;
			m_TotalArraySize += m_PositionArraySize;
		}
		else
		{
			m_PositionArraySize = 0;
		}

		if (m_VertexAttributes & VERTEX_COLOR_ATTRIBUTE)
		{
			m_ColorArraySize = m_PackedVerticesNum * VERTEX_COLOR_ATTRIBUTE_SIZE * 4;
			m_TotalArraySize += m_ColorArraySize;
		}
		else
		{
			m_ColorArraySize = 0;
		}

		if (m_VertexAttributes & VERTEX_TEXCOORD_ATTRIBUTE)
		{
			m_TexcoordArraySize = m_PackedVerticesNum * VERTEX_TEXCOORD_ATTRIBUTE_SIZE * 4;
			m_TotalArraySize += m_TexcoordArraySize;
		}
		else
		{
			m_TexcoordArraySize = 0;
		}

		if (m_VertexAttributes & VERTEX_NORMAL_ATTRIBUTE)
		{
			m_NormalArraySize = m_PackedVerticesNum * VERTEX_NORMAL_ATTRIBUTE_SIZE * 4;
			m_TotalArraySize += m_NormalArraySize;
		}
		else
		{
			m_NormalArraySize = 0;
		}

		if (m_VertexAttributes & VERTEX_TANGENT_ATTRIBUTE)
		{
			m_TangentArraySize = m_PackedVerticesNum * VERTEX_TANGENT_ATTRIBUTE_SIZE * 4;
			m_TotalArraySize += m_TangentArraySize;
		}
		else
		{
			m_TangentArraySize = 0;
		}

		m_OldIndexArraySize = m_IndexArraySize;
		m_IndexArraySize	= m_PackedTrianglesNum * 3 * 2;
	}

	void TrianglesRenderData::UpdateBuffers(void)
	{
		u32 array_offset = 0;

		if (m_ArrayBufferId == 0)
		{
			glGenBuffers(1, &m_ArrayBufferId);
			GRAPHICS_ASSERT_GL_ERROR();
		}

		glBindBuffer(GL_ARRAY_BUFFER, m_ArrayBufferId);

		// Allocate ARRAY_BUFFER.
		if (m_TotalArraySize > m_OldTotalArraySize)
		{
			glBufferData(ARRAY_BUFFER_DATA_TYPE, m_TotalArraySize, 0, GL_STATIC_DRAW);

			m_OldTotalArraySize = m_TotalArraySize;
		}

		// Copy POSITION.
		if (m_VertexAttributes & VERTEX_POSITION_ATTRIBUTE)
		{
			glBufferSubData(GL_ARRAY_BUFFER, array_offset, m_PositionArraySize, m_PositionArray);
			array_offset += m_PositionArraySize;
		}

		// Copy COLOR.
		if (m_VertexAttributes & VERTEX_COLOR_ATTRIBUTE)
		{
			glBufferSubData(GL_ARRAY_BUFFER, array_offset, m_ColorArraySize, m_ColorArray);
			array_offset += m_ColorArraySize;
		}

		// Copy TEXCOORD.
		if (m_VertexAttributes & VERTEX_TEXCOORD_ATTRIBUTE)
		{
			glBufferSubData(GL_ARRAY_BUFFER, array_offset, m_TexcoordArraySize, m_TexcoordArray);
			array_offset += m_TexcoordArraySize;
		}

		// Copy NORMAL.
		if (m_VertexAttributes & VERTEX_NORMAL_ATTRIBUTE)
		{
			glBufferSubData(GL_ARRAY_BUFFER, array_offset, m_NormalArraySize, m_NormalArray);
			array_offset += m_NormalArraySize;
		}

		// Copy TANGENT.
		if (m_VertexAttributes & VERTEX_TANGENT_ATTRIBUTE)
		{
			glBufferSubData(GL_ARRAY_BUFFER, array_offset, m_TangentArraySize, m_TangentArray);
			array_offset += m_TangentArraySize;
		}

		if (m_ElementArrayBufferId == 0)
		{
			glGenBuffers(1, &m_ElementArrayBufferId);
			GRAPHICS_ASSERT_GL_ERROR();
		}

		GRAPHICS_ASSERT_GL_ERROR();

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ElementArrayBufferId);

		// Allocate ELEMENT_ARRAY_BUFFER.
		if (m_IndexArraySize > m_OldIndexArraySize)
		{
			glBufferData(ELEMENT_ARRAY_BUFFER_DATA_TYPE, m_IndexArraySize, NULL, GL_STATIC_DRAW);
			m_OldIndexArraySize = m_IndexArraySize;

			GRAPHICS_ASSERT_GL_ERROR();
		}

		// Copy ELEMENT_ARRAY_BUFFER.
		glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, m_IndexArraySize, m_IndexArray);

		GRAPHICS_ASSERT_GL_ERROR();

		m_InitBufferFlag = true;
	}

	void TrianglesRenderData::SetUpdateBufferBeforeDraw(const bool updateBufferFlag) { m_UpdateBufferFlag = updateBufferFlag; }

	void TrianglesRenderData::DestroyBuffers(void)
	{
		if (!m_InitBufferFlag)
		{
			return;
		}

		GRAPHICS_ASSERT_GL_ERROR();

		if (m_ArrayBufferId != 0)
		{
			glDeleteBuffers(1, &m_ArrayBufferId);
			m_ArrayBufferId = 0;
		}

		if (m_ElementArrayBufferId != 0)
		{
			glDeleteBuffers(1, &m_ElementArrayBufferId);
			m_ElementArrayBufferId = 0;
		}

		GRAPHICS_ASSERT_GL_ERROR();

		m_InitBufferFlag = false;
	}

	void TrianglesRenderData::SetPosition(const u32 index, const GLfloat positionX, const GLfloat positionY, const GLfloat positionZ)
	{
		if ((m_VertexAttributes & VERTEX_POSITION_ATTRIBUTE) && (index < m_VerticesNum))
		{
			m_PositionArray[4 * index]	   = positionX;
			m_PositionArray[4 * index + 1] = positionY;
			m_PositionArray[4 * index + 2] = positionZ;
			m_PositionArray[4 * index + 3] = 1.0f;
		}
		else
		{
			NN_TPANIC_("Invalid SetPosition().\n");
		}
	}

	void TrianglesRenderData::SetPosition(const u32		index,
										  const GLfloat positionX,
										  const GLfloat positionY,
										  const GLfloat positionZ,
										  const GLfloat positionW)
	{
		if ((m_VertexAttributes & VERTEX_POSITION_ATTRIBUTE) && (index < m_VerticesNum))
		{
			m_PositionArray[4 * index]	   = positionX;
			m_PositionArray[4 * index + 1] = positionY;
			m_PositionArray[4 * index + 2] = positionZ;
			m_PositionArray[4 * index + 3] = positionW;
		}
		else
		{
			NN_TPANIC_("Invalid SetPosition().\n");
		}
	}

	void TrianglesRenderData::SetColor(const u32 index, const GLfloat red, const GLfloat green, const GLfloat blue)
	{
		if ((m_VertexAttributes & VERTEX_COLOR_ATTRIBUTE) && (index < m_VerticesNum))
		{
			m_ColorArray[4 * index]		= red;
			m_ColorArray[4 * index + 1] = green;
			m_ColorArray[4 * index + 2] = blue;
			m_ColorArray[4 * index + 3] = 1.0f;
		}
		else
		{
			NN_TPANIC_("Invalid SetColor().\n");
		}
	}

	void TrianglesRenderData::SetColor(const u32 index, const GLfloat red, const GLfloat green, const GLfloat blue, const GLfloat alpha)
	{
		if ((m_VertexAttributes & VERTEX_COLOR_ATTRIBUTE) && (index < m_VerticesNum))
		{
			m_ColorArray[4 * index]		= red;
			m_ColorArray[4 * index + 1] = green;
			m_ColorArray[4 * index + 2] = blue;
			m_ColorArray[4 * index + 3] = alpha;
		}
		else
		{
			NN_TPANIC_("Invalid SetColor().\n");
		}
	}

	void TrianglesRenderData::SetColor(const GLfloat red, const GLfloat green, const GLfloat blue)
	{
		if (m_VertexAttributes & VERTEX_COLOR_ATTRIBUTE)
		{
			for (u32 index = 0; index < m_VerticesNum; index++)
			{
				m_ColorArray[4 * index]		= red;
				m_ColorArray[4 * index + 1] = green;
				m_ColorArray[4 * index + 2] = blue;
				m_ColorArray[4 * index + 3] = 1.0f;
			}
		}
		else
		{
			NN_TPANIC_("Invalid SetColor().\n");
		}
	}

	void TrianglesRenderData::SetColor(const GLfloat red, const GLfloat green, const GLfloat blue, const GLfloat alpha)
	{
		if (m_VertexAttributes & VERTEX_COLOR_ATTRIBUTE)
		{
			for (u32 index = 0; index < m_VerticesNum; index++)
			{
				m_ColorArray[4 * index]		= red;
				m_ColorArray[4 * index + 1] = green;
				m_ColorArray[4 * index + 2] = blue;
				m_ColorArray[4 * index + 3] = alpha;
			}
		}
		else
		{
			NN_TPANIC_("Invalid SetColor().\n");
		}
	}

	void TrianglesRenderData::SetTexcoord(const u32 index, const GLfloat texcoordS, const GLfloat texcoordT)
	{
		if ((m_VertexAttributes & VERTEX_TEXCOORD_ATTRIBUTE) && (index < m_VerticesNum))
		{
			m_TexcoordArray[3 * index]	   = texcoordS;
			m_TexcoordArray[3 * index + 1] = texcoordT;
			m_TexcoordArray[3 * index + 2] = 1.0f;
		}
		else
		{
			NN_TPANIC_("Invalid SetTexcoord().\n");
		}
	}

	void TrianglesRenderData::SetTexcoord(const u32 index, const GLfloat texcoordS, const GLfloat texcoordT, const GLfloat texcoordR)
	{
		if ((m_VertexAttributes & VERTEX_TEXCOORD_ATTRIBUTE) && (index < m_VerticesNum))
		{
			m_TexcoordArray[3 * index]	   = texcoordS;
			m_TexcoordArray[3 * index + 1] = texcoordT;
			m_TexcoordArray[3 * index + 2] = texcoordR;
		}
		else
		{
			NN_TPANIC_("Invalid SetTexcoord().\n");
		}
	}

	void TrianglesRenderData::SetNormal(const u32 index, const GLfloat normalX, const GLfloat normalY, const GLfloat normalZ)
	{
		if ((m_VertexAttributes & VERTEX_NORMAL_ATTRIBUTE) && (index < m_VerticesNum))
		{
			m_NormalArray[3 * index]	 = normalX;
			m_NormalArray[3 * index + 1] = normalY;
			m_NormalArray[3 * index + 2] = normalZ;
		}
		else
		{
			NN_TPANIC_("Invalid SetNormal().\n");
		}
	}

	void TrianglesRenderData::SetTangent(const u32 index, const GLfloat tangentX, const GLfloat tangentY, const GLfloat tangentZ)
	{
		if ((m_VertexAttributes & VERTEX_TANGENT_ATTRIBUTE) && (index < m_VerticesNum))
		{
			m_TangentArray[3 * index]	  = tangentX;
			m_TangentArray[3 * index + 1] = tangentY;
			m_TangentArray[3 * index + 2] = tangentZ;
		}
		else
		{
			NN_TPANIC_("Invalid SetTangent().\n");
		}
	}

	void TrianglesRenderData::SetIndex(const u32 index, const GLuint index0, const GLuint index1, const GLuint index2)
	{
		if (index < m_TrianglesNum)
		{
			m_IndexArray[3 * index]		= static_cast<GLushort>(index0);
			m_IndexArray[3 * index + 1] = static_cast<GLushort>(index1);
			m_IndexArray[3 * index + 2] = static_cast<GLushort>(index2);
		}
		else
		{
			NN_TPANIC_("Invalid SetIndex().\n");
		}
	}

	void TrianglesRenderData::SetSquareIndex(const u32	  squareIndex,
											 const GLuint index0,
											 const GLuint index1,
											 const GLuint index2,
											 const GLuint index3)
	{
		u32 squareIndex6 = 6 * squareIndex;

		if (squareIndex6 <= m_TrianglesNum)
		{
			// Degenerate triangle.
			if (squareIndex != 0)
			{
				m_IndexArray[squareIndex6 - 1] = static_cast<GLushort>(index0);
			}

			m_IndexArray[squareIndex6]	   = static_cast<GLushort>(index0);
			m_IndexArray[squareIndex6 + 1] = static_cast<GLushort>(index1);
			m_IndexArray[squareIndex6 + 2] = static_cast<GLushort>(index2);
			m_IndexArray[squareIndex6 + 3] = static_cast<GLushort>(index3);

			// Degenerate triangle.
			m_IndexArray[squareIndex6 + 4] = static_cast<GLushort>(index3);
			m_IndexArray[squareIndex6 + 5] = static_cast<GLushort>(index3);
		}
		else
		{
			NN_TPANIC_("Invalid SetSquareIndex().\n");
		}
	}

	void TrianglesRenderData::SetWorldPosition(const f32 worldPositionX, const f32 worldPositionY, const f32 worldPositionZ)
	{
		m_PositionX = worldPositionX;
		m_PositionY = worldPositionY;
		m_PositionZ = worldPositionZ;
	}

	void TrianglesRenderData::TranslateWorldPosition(const f32 worldPositionX, const f32 worldPositionY, const f32 worldPositionZ)
	{
		m_PositionX += worldPositionX;
		m_PositionY += worldPositionY;
		m_PositionZ += worldPositionZ;
	}

	void TrianglesRenderData::GetWorldPosition(f32& worldPositionX, f32& worldPositionY, f32& worldPositionZ)
	{
		worldPositionX = m_PositionX;
		worldPositionY = m_PositionY;
		worldPositionZ = m_PositionZ;
	}

	void TrianglesRenderData::SetWorldAngle(const f32 worldAngleX, const f32 worldAngleY, const f32 worldAngleZ)
	{
		m_AngleX = worldAngleX;
		m_AngleY = worldAngleY;
		m_AngleZ = worldAngleZ;
	}

	void TrianglesRenderData::RotateWorldAngle(const f32 worldAngleX, const f32 worldAngleY, const f32 worldAngleZ)
	{
		m_AngleX += worldAngleX;
		m_AngleY += worldAngleY;
		m_AngleZ += worldAngleZ;
	}

	void TrianglesRenderData::GetWorldAngle(f32& worldAngleX, f32& worldAngleY, f32& worldAngleZ)
	{
		worldAngleX = m_AngleX;
		worldAngleY = m_AngleY;
		worldAngleZ = m_AngleZ;
	}

	nn::math::MTX44 TrianglesRenderData::GetWorldMatrix(void) const
	{
		nn::math::Vector3 positionVector(m_PositionX, m_PositionY, m_PositionZ);
		nn::math::MTX34	  positionMatrix;
		MTX34Identity(&positionMatrix);
		MTX34Translate(&positionMatrix, &positionVector);
		nn::math::MTX44 worldMatrix(positionMatrix);

		nn::math::MTX44 worldRotMatrix;
		MTX44RotXYZDeg(&worldRotMatrix, m_AngleX, m_AngleY, m_AngleZ);
		MTX44Mult(&worldMatrix, &worldMatrix, &worldRotMatrix);

		return worldMatrix;
	}

	void TrianglesRenderData::Draw(void)
	{
		if (!m_InitArrayFlag)
		{
			return;
		}

		if (m_UpdateBufferFlag)
		{
			UpdateBuffers();
		}

		// Setup vertex buffers.
		glBindBuffer(GL_ARRAY_BUFFER, m_ArrayBufferId);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ElementArrayBufferId);

		if (m_UpdateAttributesFlag)
		{
			EnableVertexAttributes();
		}

		DrawElements();
	}

	void TrianglesRenderData::DrawElements(void)
	{
		GRAPHICS_ASSERT_GL_ERROR();

		if (m_TriangleType == GL_TRIANGLES)
		{
			u32 indices_num = m_PackedTrianglesNum * 3;
			glDrawElements(GL_TRIANGLES, indices_num, GL_UNSIGNED_SHORT, 0);
		}
		else if (m_TriangleType == GL_TRIANGLE_STRIP)
		{
			u32 indices_num = 6 * (m_PackedTrianglesNum / 4);
			glDrawElements(GL_TRIANGLE_STRIP, indices_num, GL_UNSIGNED_SHORT, 0);
		}

		GRAPHICS_ASSERT_GL_ERROR();
	}

	void TrianglesRenderData::SetEnableVertexAttributesBeforeDraw(const bool updateAttributesFlag)
	{
		m_UpdateAttributesFlag = updateAttributesFlag;
	}

	void TrianglesRenderData::EnableVertexAttributes(void)
	{
		u32 attrib_index = 0;
		u32 array_offset = 0;

		GRAPHICS_ASSERT_GL_ERROR();

		// POSITION
		if (m_VertexAttributes & VERTEX_POSITION_ATTRIBUTE)
		{
			glEnableVertexAttribArray(attrib_index);
			glVertexAttribPointer(attrib_index, VERTEX_POSITION_ATTRIBUTE_SIZE, GL_FLOAT, GL_FALSE, 0, 0);
			attrib_index += 1;
			array_offset += m_PositionArraySize;
		}

		// COLOR
		if (m_VertexAttributes & VERTEX_COLOR_ATTRIBUTE)
		{
			glEnableVertexAttribArray(attrib_index);
			glVertexAttribPointer(attrib_index, VERTEX_COLOR_ATTRIBUTE_SIZE, GL_FLOAT, GL_FALSE, 0, (GLvoid*)array_offset);

			attrib_index += 1;
			array_offset += m_ColorArraySize;
		}

		// TEXCOORD
		if (m_VertexAttributes & VERTEX_TEXCOORD_ATTRIBUTE)
		{
			glEnableVertexAttribArray(attrib_index);
			glVertexAttribPointer(attrib_index, VERTEX_TEXCOORD_ATTRIBUTE_SIZE, GL_FLOAT, GL_FALSE, 0, (GLvoid*)array_offset);

			attrib_index += 1;
			array_offset += m_TexcoordArraySize;
		}

		// NORMAL
		if (m_VertexAttributes & VERTEX_NORMAL_ATTRIBUTE)
		{
			glEnableVertexAttribArray(attrib_index);
			glVertexAttribPointer(attrib_index, VERTEX_NORMAL_ATTRIBUTE_SIZE, GL_FLOAT, GL_FALSE, 0, (GLvoid*)array_offset);

			attrib_index += 1;
			array_offset += m_NormalArraySize;
		}

		// TANGENT
		if (m_VertexAttributes & VERTEX_TANGENT_ATTRIBUTE)
		{
			glEnableVertexAttribArray(attrib_index);
			glVertexAttribPointer(attrib_index, VERTEX_TANGENT_ATTRIBUTE_SIZE, GL_FLOAT, GL_FALSE, 0, (GLvoid*)array_offset);

			attrib_index += 1;
			array_offset += m_TangentArraySize;
		}

		for (u32 i = attrib_index; i < 8; i++)
		{
			glDisableVertexAttribArray(i);
		}

		GRAPHICS_ASSERT_GL_ERROR();
	}

}}	// namespace np::graphics
