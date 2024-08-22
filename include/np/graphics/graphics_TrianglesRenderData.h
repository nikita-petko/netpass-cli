#ifndef NP_GRAPHICS_GRAPHICS_TRIANGLES_RENDER_DATA_H_
#define NP_GRAPHICS_GRAPHICS_TRIANGLES_RENDER_DATA_H_

#include <nn/gx.h>
#include <nn/math.h>

#include <np/graphics/graphics_MemoryManager.h>
#include <np/graphics/graphics_RenderData.h>

namespace np { namespace graphics {
	/*!
		@brief Enumerates types that represent vertex attributes.
	*/
	enum VertexAttributes
	{
		VERTEX_NONE_ATTRIBUTE	  = 0,		   //!< Specifies that vertices have no attributes.
		VERTEX_POSITION_ATTRIBUTE = (1 << 0),  //!< Specifies the position coordinate attributes of the vertices.
		VERTEX_COLOR_ATTRIBUTE	  = (1 << 1),  //!< Specifies the vertex color attributes.
		VERTEX_TEXCOORD_ATTRIBUTE = (1 << 2),  //!< Specifies the vertex texture coordinate attributes.
		VERTEX_NORMAL_ATTRIBUTE	  = (1 << 3),  //!< Specifies vertex normal attributes.
		VERTEX_TANGENT_ATTRIBUTE  = (1 << 4)   //!< Specifies vertex tangent attributes.
	};

	/*!
		@brief Specifies the number of elements for the vertex position coordinates (in the form xyzw, with all elements of type f32).
	*/
	const int VERTEX_POSITION_ATTRIBUTE_SIZE = 4;

	/*!
		@brief Specifies the number of vertex color elements (in the format red, green, blue, and alpha, with all elements of type
		f32).
	*/
	const int VERTEX_COLOR_ATTRIBUTE_SIZE = 4;

	/*!
		@brief Specifies the number of elements for the vertex texture coordinates (all elements of type f32, in the format s, t, and
		r).
	*/
	const int VERTEX_TEXCOORD_ATTRIBUTE_SIZE = 3;

	/*!
		@brief Specifies the number of elements in a vertex normal (all elements of type f32, with values nx, ny, and nz).
	*/
	const int VERTEX_NORMAL_ATTRIBUTE_SIZE = 3;

	/*!
		@brief Specifies the number of elements in a vertex tangent (all elements of type f32, with values tx, ty, and tz).
	*/
	const int VERTEX_TANGENT_ATTRIBUTE_SIZE = 3;

	/*!
		@brief Specifies the maximum number of triangles that the TrianglesRenderData class can have as rendering data.
	*/
	const u32 MAX_TRIANGLES_NUM = 8192;

	/*!
		@brief Specifies the maximum number of quadrangles that the TrianglesRenderData class can have as rendering data.
	*/
	const u32 MAX_SQUARES_NUM = MAX_TRIANGLES_NUM / 2;

	/*!
		@brief Specifies the maximum number of triangle vertices that the TrianglesRenderData class can have as rendering data.
	*/
	const u32 MAX_TRIANGLE_VERTICES_NUM = MAX_TRIANGLES_NUM * 3;

	/*!
		@brief Specifies the maximum number of triangle indices that the TrianglesRenderData class can have as rendering data.
	*/
	const u32 MAX_INDICES_NUM = MAX_TRIANGLES_NUM * 3;

	/*!
		@brief Specifies the data type of vertex buffers.
	*/
	const GLenum ARRAY_BUFFER_DATA_TYPE = GL_ARRAY_BUFFER;

	/*!
		@brief Specifies the data type of index buffers.
	*/
	const GLenum ELEMENT_ARRAY_BUFFER_DATA_TYPE = GL_ELEMENT_ARRAY_BUFFER;

	/*!
		@brief Represents the triangle mesh that is the rendering data.
	*/
	class TrianglesRenderData : public RenderData
	{
	public:
		/*!
			@brief Constructor.
		*/
		TrianglesRenderData(void);

		/*!
			@brief Destructor.
		*/
		virtual ~TrianglesRenderData(void);

	public:
		/*!
			@brief Initializes the rendering data.
		*/
		virtual void Initialize(void);

		/*!
			@brief Initializes the rendering data.
		*/
		virtual void Finalize(void);

	public:
		/*!
			@brief Initializes the vertex buffers.

			@param[in] vertexAttributes Specifies the logical sum of vertex attributes.
			@param[in] triangleType Specifies the type of triangle primitive to render.
			@param[in] verticesNum Specifies the total number of the vertices of the triangular primitive.
			@param[in] trianglesNum Specifies the total number of triangular primitives.
		*/
		void InitializeVertexBuffers(const u32 vertexAttributes, const GLenum triangleType, const u32 verticesNum, const u32 trianglesNum);

	protected:
		void SetVertexAttributes(const u32 vertexAttributes);
		void SetTriangleType(const GLenum triangleType);
		void SetVerticesNum(const u32 verticesNum);
		void SetTrianglesNum(const u32 trianglesNum);
		void CreateArrays(void);
		void DestroyArrays(void);

	public:
		/*!
			@brief Sets the number of packed vertices.

			@param[in] packedVerticesNum Specifies the number of packed vertices.
		*/
		void SetPackedVerticesNum(const u32 packedVerticesNum);

		/*!
			@brief Adds the number of packed vertices.

			@param[in] packedVerticesNum Specifies the number of packed vertices to add.
		*/
		void AddPackedVerticesNum(const u32 packedVerticesNum);

		/*!
			@brief Gets the number of packed vertices.

			@return Returns the number of packed vertices.
		*/
		u32 GetPackedVerticesNum(void) const;

	public:
		/*!
			@brief Sets the number of packed triangles.

			@param[in] packedTrianglesNum Specifies the number of packed triangles.
		*/
		void SetPackedTrianglesNum(const u32 packedTrianglesNum);

		/*!
			@brief Adds the number of packed triangles.

			@param[in] packedTrianglesNum Specifies the number of packed triangles to add.
		*/
		void AddPackedTrianglesNum(const u32 packedTrianglesNum);

		/*!
			@brief Gets the number of packed triangles.

			@return Returns the number of packed triangles.
		*/
		u32 GetPackedTrianglesNum(void) const;

	public:
		/*!
			@brief Draws the packed triangles.
		*/
		void DrawPackedTriangles(void);

	public:
		/*!
			@brief Sets the number of packed triangles to zero.
		*/
		void ClearPackedNum(void);

		/*!
			@brief Finds the size of the vertex buffer to update based on the packed triangle data.
		*/
		void SetPackedArraysSize(void);

	public:
		/*!
			@brief Updates the vertex buffers.
		*/
		void UpdateBuffers(void);

		/*!
			@brief Sets whether to update the vertex buffer before each draw operation.
		*/
		void SetUpdateBufferBeforeDraw(const bool updateBufferFlag);

	protected:
		void DestroyBuffers(void);

	public:
		/*!
			@brief Sets a position coordinate in the vertex array.

			@param[in] index Specifies the triangle index.
			@param[in] positionX Specifies the x-coordinate of position coordinates.
			@param[in] positionY Specifies the y-coordinate of position coordinates.
			@param[in] positionZ Specifies the z-coordinate of position coordinates.
		*/
		void SetPosition(const u32 index, const GLfloat positionX, const GLfloat positionY, const GLfloat positionZ);

		/*!
			@brief Sets a position coordinate in the vertex array.

			@param[in] index Specifies the triangle index.
			@param[in] positionX Specifies the x-coordinate of position coordinates.
			@param[in] positionY Specifies the y-coordinate of position coordinates.
			@param[in] positionZ Specifies the z-coordinate of position coordinates.
			@param[in] positionW Specifies the w-coordinate of position coordinates.
		*/
		void SetPosition(const u32	   index,
						 const GLfloat positionX,
						 const GLfloat positionY,
						 const GLfloat positionZ,
						 const GLfloat positionW);

		/*!
			@brief Sets a vertex color in the vertex array.

			@param[in] index Specifies the triangle index.
			@param[in] red Specifies the red component of the vertex color.
			@param[in] green Specifies the green component of the vertex color.
			@param[in] blue Specifies the blue component of the vertex color.
		*/
		void SetColor(const u32 index, const GLfloat red, const GLfloat green, const GLfloat blue);

		/*!
			@brief Sets a vertex color in the vertex array.

			@param[in] index Specifies the triangle index.
			@param[in] red Specifies the red component of the vertex color.
			@param[in] green Specifies the green component of the vertex color.
			@param[in] blue Specifies the blue component of the vertex color.
			@param[in] alpha Specifies the alpha component of the vertex color.
		*/
		void SetColor(const u32 index, const GLfloat red, const GLfloat green, const GLfloat blue, const GLfloat alpha);

		/*!
			@brief Sets a vertex color in the vertex array.

			@param[in] red Specifies the red component of the vertex color.
			@param[in] green Specifies the green component of the vertex color.
			@param[in] blue Specifies the blue component of the vertex color.
		*/
		virtual void SetColor(const GLfloat red, const GLfloat green, const GLfloat blue);

		/*!
			@brief Sets a vertex color in the vertex array.

			@param[in] index Specifies the triangle index.
			@param[in] red Specifies the red component of the vertex color.
			@param[in] green Specifies the green component of the vertex color.
			@param[in] blue Specifies the blue component of the vertex color.
			@param[in] alpha Specifies the alpha component of the vertex color.
		*/
		virtual void SetColor(const GLfloat red, const GLfloat green, const GLfloat blue, const GLfloat alpha);

		/*!
			@brief Sets a texture coordinate in the vertex array.

			@param[in] index Specifies the triangle index.
			@param[in] texcoordS Specifies the s-coordinate of the texture coordinates.
			@param[in] texcoordT Specifies the t-coordinate of the texture coordinates.
		*/
		void SetTexcoord(const u32 index, const GLfloat texcoordS, const GLfloat texcoordT);

		/*!
			@brief Sets a texture coordinate in the vertex array.

			@param[in] index Specifies the triangle index.
			@param[in] texcoordS Specifies the s-coordinate of the texture coordinates.
			@param[in] texcoordT Specifies the t-coordinate of the texture coordinates.
			@param[in] texcoordR Specifies the r-coordinate of the texture coordinates.
		*/
		void SetTexcoord(const u32 index, const GLfloat texcoordS, const GLfloat texcoordT, const GLfloat texcoordR);

		/*!
			@brief Sets a normal in the vertex array.

			@param[in] index Specifies the triangle index.
			@param[in] normalX Specifies the x-coordinate of the normal.
			@param[in] normalY Specifies the y-coordinate of the normal.
			@param[in] normalZ Specifies the z-coordinate of the normal.
		*/
		void SetNormal(const u32 index, const GLfloat normalX, const GLfloat normalY, const GLfloat normalZ);

		/*!
			@brief Sets a tangent in the vertex array.

			@param[in] index Specifies the triangle index.
			@param[in] tangentX Specifies the x-coordinate of the tangent.
			@param[in] tangentY Specifies the y-coordinate of the tangent.
			@param[in] tangentZ Specifies the z-coordinate of the tangent.
		*/
		void SetTangent(const u32 index, const GLfloat tangentX, const GLfloat tangentY, const GLfloat tangentZ);

		/*!
			@brief Sets the indices of a triangle in the index array.

			@param[in] index Specifies the triangle index.
			@param[in] index0 Specifies the index of the first vertex of the triangle.
			@param[in] index1 Specifies the index of the second vertex of the triangle.
			@param[in] index2 Specifies the index of the third vertex of the triangle.
		*/
		void SetIndex(const u32 index, const GLuint index0, const GLuint index1, const GLuint index2);

		/*!
			@brief Sets the index to create a TRIANGLE_STRIP quadrangle in the index array.

			@param[in] index Specifies the quadrangle index.
			@param[in] index0 Specifies the index of the first vertex of the quadrangle.
			@param[in] index1 Specifies the index of the second vertex of the quadrangle.
			@param[in] index2 Specifies the index of the third vertex of the quadrangle.
			@param[in] index3 Specifies the index of the fourth vertex of the quadrangle.
		*/
		void SetSquareIndex(const u32 square_index, const GLuint index0, const GLuint index1, const GLuint index2, const GLuint index3);

	public:
		/*!
			@brief Sets the world coordinates.

			@param[in] worldPositionX Specifies the x-coordinate of world coordinates.
			@param[in] worldPositionY Specifies the y-coordinate of world coordinates.
			@param[in] worldPositionZ Specifies the z-coordinate of world coordinates.
		*/
		void SetWorldPosition(const f32 worldPositionX, const f32 worldPositionY, const f32 worldPositionZ);

		/*!
			@brief Translates the position by the distance specified by the world coordinate vector.

			@param[in] worldPositionX Specifies the x-coordinate of the world coordinate vector.
			@param[in] worldPositionY Specifies the y-coordinate of the world coordinate vector.
			@param[in] worldPositionZ Specifies the z-coordinate of the world coordinate vector.
		*/
		void TranslateWorldPosition(const f32 worldPositionX, const f32 worldPositionY, const f32 worldPositionZ);

		/*!
			@brief Gets the world coordinates.

			@param[out] worldPositionX Specifies the x-coordinate of world coordinates.
			@param[out] worldPositionY Specifies the y-coordinate of world coordinates.
			@param[out] worldPositionZ Specifies the z-coordinate of world coordinates.
		*/
		void GetWorldPosition(f32& worldPositionX, f32& worldPositionY, f32& worldPositionZ);

		/*!
			@brief Sets the rotation angle in the world coordinate system.

			@param[in] worldAngleX Specifies the angle around the x-axis.
			@param[in] worldAngleY Specifies the angle around the y-axis.
			@param[in] worldAngleZ Specifies the angle around the z-axis.
		*/
		void SetWorldAngle(const f32 worldAngleX, const f32 worldAngleY, const f32 worldAngleZ);

		/*!
			@brief Rotate by the specified angle of rotation in the world coordinate system.

			@param[in] worldAngleX Specifies the angle around the x-axis in world coordinates.
			@param[in] worldAngleY Specifies the angle around the y-axis in world coordinates.
			@param[in] worldAngleZ Specifies the angle around the z-axis in world coordinates.
		*/
		void RotateWorldAngle(const f32 worldAngleX, const f32 worldAngleY, const f32 worldAngleZ);

		/*!
			@brief Gets the angle of rotation in the world coordinate system.

			@param[in] worldAngleX Specifies the angle around the x-axis.
			@param[in] worldAngleY Specifies the angle around the y-axis.
			@param[in] worldAngleZ Specifies the angle around the z-axis.
		*/
		void GetWorldAngle(f32& worldAngleX, f32& worldAngleY, f32& worldAngleZ);

	public:
		/*!
			@brief Gets the world matrix.

			@return Returns the world matrix.
		*/
		virtual nn::math::MTX44 GetWorldMatrix(void) const;

	public:
		/*!
			@brief Performs rendering using the rendering data.
		*/
		virtual void Draw(void);

	protected:
		virtual void DrawElements(void);

	public:
		/*!
			@brief Enables vertex attributes.
		*/
		virtual void EnableVertexAttributes(void);

		/*!
			@brief Sets whether to enable vertex attributes before rendering.

			@param[in] updateAttributesFlag Specify true if vertex attributes are enabled before rendering, and false
			otherwise.
		*/
		void SetEnableVertexAttributesBeforeDraw(const bool updateAttributesFlag);

	protected:
		u32	   m_ShaderType;
		u32	   m_VertexAttributes;
		GLenum m_TriangleType;
		u32	   m_VerticesNum;
		u32	   m_TrianglesNum;
		u32	   m_PackedVerticesNum;
		u32	   m_PackedTrianglesNum;

	protected:
		u32 m_PositionArraySize;
		u32 m_ColorArraySize;
		u32 m_TexcoordArraySize;
		u32 m_NormalArraySize;
		u32 m_TangentArraySize;
		u32 m_TotalArraySize;
		u32 m_OldTotalArraySize;

		u32 m_IndexArraySize;
		u32 m_OldIndexArraySize;

	protected:
		GLfloat*  m_PositionArray;
		GLfloat*  m_ColorArray;
		GLfloat*  m_TexcoordArray;
		GLfloat*  m_NormalArray;
		GLfloat*  m_TangentArray;
		GLushort* m_IndexArray;

	protected:
		GLuint m_ArrayBufferId;
		GLuint m_ElementArrayBufferId;

	protected:
		bool m_InitArrayFlag;
		bool m_InitBufferFlag;
		bool m_UpdateBufferFlag;
		bool m_UpdateAttributesFlag;

	protected:
		f32 m_PositionX;
		f32 m_PositionY;
		f32 m_PositionZ;
		f32 m_AngleX;
		f32 m_AngleY;
		f32 m_AngleZ;
	};
}}	// namespace np::graphics

#endif
