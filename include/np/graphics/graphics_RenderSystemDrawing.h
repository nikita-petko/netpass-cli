#ifndef NP_GRAPHICS_GRAPHICS_RENDER_SYSTEM_DRAWING_H_
#define NP_GRAPHICS_GRAPHICS_RENDER_SYSTEM_DRAWING_H_

#include <np/graphics/graphics_GraphicsDrawing.h>
#include <np/graphics/graphics_RenderSystem.h>
#include <np/graphics/graphics_Utility.h>

namespace np { namespace graphics {

	/*!
		@brief  Indicates the maximum number of texture objects that the GraphicsDrawing class can maintain simultaneously.
	*/
	const u32 MAX_TEXTURES_NUM = 16;

	/*!
		@brief  Indicates the maximum number of triangles that the GraphicsDrawing class can hold as rendering data when texture
	mapping.
	*/
	const u32 MAX_TEXTURED_TRIANGLES_NUM = 64;

	/*!
		@brief  Indicates the maximum number of triangle indices that the GraphicsDrawing class can hold as rendering data when texture
	mapping.
	*/
	const u32 MAX_TEXTURED_INDICES_NUM = MAX_TEXTURED_TRIANGLES_NUM * 3;

	/*!
		@brief  This class performs simple 2D drawing operations on the CTR.
	*/
	class RenderSystemDrawing : public graphics::RenderSystem
	{
	public:
		RenderSystemDrawing(void);
		virtual ~RenderSystemDrawing(void);

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
		/*!
			@brief Initializes 2D triangles for drawing.

			This function does not need to be called explicitly because it is implicitly called by the DrawTriangle2d
			function. Only call this function if you want to change the maximum number of 2D triangles to draw on the upper and lower LCD
			screens. Call it immediately after calling the Initialize function.

			@param[in] maxTrianglesNum Specifies the maximum number of triangles to render on each LCD, both upper and lower. (The default
		   is MAX_TRIANGLES_NUM.)
		*/
		void InitializeTriangles(const u32 maxTrianglesNum = np::graphics::MAX_TRIANGLES_NUM);

		/*!
			@brief  Initializes 2D quadrangles for drawing.

			This function does not need to be explicitly called because it is implicitly called inside the DrawSquare2d
			function. Only call this function if you want to change the maximum number of 2D quadrangles (including points and lines) to
		   draw on the upper and lower LCD screens. Call it immediately after calling the Initialize function.

			@param[in] maxSquaresNum Specifies the maximum number of quadrangles to render on each LCD, both upper and lower. (The default
		   is MAX_SQUARES_NUM.)
		*/
		void InitializeSquares(const u32 maxSquaresNum = np::graphics::MAX_SQUARES_NUM);

		/*!
			@brief  Initializes 2D text for drawing.

			This function does not need to be explicitly called because it is implicitly called inside the DrawText2d
			function. Only call this function if you want to change the maximum number of 2D text characters to draw on the upper and lower
		   LCD screens. Call it immediately after calling the Initialize function.

			@param[in] maxLength Specifies the maximum number of characters to render on each LCD, both upper and lower. (The default is
			MAX_TEXT_NUM.)
		*/
		void InitializeTexts(const u32 maxLength = np::graphics::MAX_TEXT_LENGTH);

		/*!
			@brief  Initializes texture-mapped 2D triangles for drawing.

			This function does not need to be explicitly called, because it is implicitly called inside the
			DrawTexturedTriangle2d function. Only call this function if you want to change the maximum number of texture-mapped 2D
			triangles to draw on the upper and lower LCD screens. Call it immediately after calling the Initialize function.

			@param[in] maxTrianglesNum Specifies the maximum number of triangles that have been texture-mapped to render on the upper and
			lower LCDs. (The default is MAX_TEXTURED_TRIANGLES_NUM.)
		*/
		void InitializeTexturedTriangles(const u32 maxTrianglesNum = MAX_TEXTURED_TRIANGLES_NUM);

		virtual void Finalize(void);

		virtual void SetViewport(const GLint x, const GLint y, const GLsizei width, const GLsizei height);

		virtual void SetClearColor(const s32 display, const f32 red, const f32 green, const f32 blue, const f32 alpha);

		virtual void Clear(const GLbitfield mask = (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT));

	public:
		/*!
			@brief Sets the vertex color of a primitive to render in 2D.

			@param[in] red Specifies the vertex color red component ([0.0f,1.0f]).
			@param[in] green Specifies the vertex color green component ([0.0f,1.0f]).
			@param[in] blue Specifies the vertex color blue component ([0.0f,1.0f]).
			@param[in] alpha Specifies the vertex color alpha component ([0.0f,1.0f]).
		*/
		virtual void SetColor(const f32 red, const f32 green, const f32 blue, const f32 alpha);

		/*!
			@brief Sets the vertex color of a primitive to render in 2D.

			@param[in] red Specifies the vertex color red component ([0.0f,1.0f]).
			@param[in] green Specifies the vertex color green component ([0.0f,1.0f]).
			@param[in] blue Specifies the vertex color blue component ([0.0f,1.0f]).
		*/
		virtual void SetColor(const f32 red, const f32 green, const f32 blue);

		/*!
			@brief Sets the depth value of a primitive to render in 2D.

			@param[in] depth2d Specifies the depth value for normalized device coordinates ([0.0f,1.0f]).
		*/
		virtual void SetDepth(const f32 depth);

		/*!
			@brief Enables automatic reduction of depth each time a primitive is drawn in 2D.

			@param[in] enableAutoDepth2d Specifies whether to automatically reduce depth (true or false).
		*/
		virtual void SetAutoDepth(const bool enableAutoDepth);

	public:
		/*!
			@brief Sets the size when drawing a point in 2D.

			@param[in] pointSize Specifies the point size in pixels ([1.0f,]).
		*/
		virtual void SetPointSize(const f32 pointSize);

		/*!
			@brief Draws a 2D point specified in window coordinates.

			@param[in] windowCoordinateX Specifies the x-coordinate of the center of the point in the window coordinate system (f32
			range).
			@param[in] windowCoordinateY Specifies the y-coordinate of the center of the point in the window coordinate system (f32
			range).
		*/
		virtual void DrawPoint(const f32 windowCoordinateX, const f32 windowCoordinateY);

	public:
		/*!
			@brief Sets the width of the lines to render in 2D.

			@param[in] lineWidth Specifies the line width in pixels ([1.0f,]).
		*/
		virtual void SetLineWidth(const f32 lineWidth);

		/*!
			@brief Draws a 2D line specified in window coordinates.

			@param[in] windowCoordinateX0 Specifies the x-coordinate of the start point of the line segment in the window coordinate system
			(f32 range).
			@param[in] windowCoordinateY0 Specifies the y-coordinate of the start point of the line segment in the window coordinate system
			(f32 range).
			@param[in] windowCoordinateX1 Specifies the x-coordinate of the end point of the line segment in the window coordinate system
			(f32 range).
			@param[in] windowCoordinateY1 Specifies the y-coordinate of the end point of the line segment in the window coordinate system
			(f32 range).
		*/
		virtual void DrawLine(const f32 windowCoordinateX0,
							  const f32 windowCoordinateY0,
							  const f32 windowCoordinateX1,
							  const f32 windowCoordinateY1);

	public:
		/*!
			@brief Draws a 2D quadrangle specified in window coordinates.

			Specify vertex data in counterclockwise order.

			@param[in] windowCoordinateX0 Specifies the x-coordinate of the top-left vertex of the quadrangle in the window coordinate
		   system (f32 range).
			@param[in] windowCoordinateY0 Specifies the y-coordinate of the top-left vertex of the quadrangle in the window coordinate
		   system (f32 range).
			@param[in] windowCoordinateX1 Specifies the x-coordinate of the bottom-left vertex of the quadrangle in the window coordinate
			system
			(f32 range).
			@param[in] windowCoordinateY1 Specifies the y-coordinate of the bottom-left vertex of the quadrangle in the window coordinate
			system
			(f32 range).
			@param[in] windowCoordinateX2 Specifies the x-coordinate of the bottom-right vertex of the quadrangle in the window coordinate
			system
			(f32 range).
			@param[in] windowCoordinateY2 Specifies the y-coordinate of the bottom-right vertex of the quadrangle in the window coordinate
			system
			(f32 range).
			@param[in] windowCoordinateX3 Specifies the x-coordinate of the top-right vertex of the quadrangle in the window coordinate
		   system (f32 range).
			@param[in] windowCoordinateY3 Specifies the y-coordinate of the top-right vertex of the quadrangle in the window coordinate
		   system (f32 range).
		*/
		virtual void FillSquare(const f32 windowCoordinateX0,
								const f32 windowCoordinateY0,
								const f32 windowCoordinateX1,
								const f32 windowCoordinateY1,
								const f32 windowCoordinateX2,
								const f32 windowCoordinateY2,
								const f32 windowCoordinateX3,
								const f32 windowCoordinateY3);

	public:
		/*!
			@brief Draws a 2D rectangle specified in window coordinates.

			@param[in] windowCoordinateX Specifies the x-coordinate of the top-left vertex of the rectangle in the window coordinate system
			(f32 range).
			@param[in] windowCoordinateY Specifies the y-coordinate of the top-left vertex of the rectangle in the window coordinate system
			(f32 range).
			@param[in] width Specifies the length of the side in the x-direction of the rectangle in the window coordinate system (f32
			range).
			@param[in] height Specifies the length of the side in the y-direction of the rectangle in the window coordinate system
			(f32 range).
		*/
		virtual void FillRectangle(const f32 windowCoordinateX, const f32 windowCoordinateY, const f32 width, const f32 height);

	public:
		/*!
			@brief Draws a 2D triangle specified in window coordinates.

			Specify vertex data in counterclockwise order.

			@param[in] windowCoordinateX0 Specifies the x-coordinate of the first vertex of the triangle in the window coordinate system
			(f32 range).
			@param[in] windowCoordinateY0 Specifies the y-coordinate of the first vertex of the triangle in the window coordinate system
			(f32 range).
			@param[in] windowCoordinateX1 Specifies the x-coordinate of the second vertex of the triangle in the window coordinate system
			(f32 range).
			@param[in] windowCoordinateY1 Specifies the y-coordinate of the second vertex of the triangle in the window coordinate system
			(f32 range).
			@param[in] windowCoordinateX2 Specifies the x-coordinate of the third vertex of the triangle in the window coordinate system
			(f32 range).
			@param[in] windowCoordinateY2 Specifies the y-coordinate of the third vertex of the triangle in the window coordinate system
			(f32 range).
		*/
		virtual void FillTriangle(const f32 windowCoordinateX0,
								  const f32 windowCoordinateY0,
								  const f32 windowCoordinateX1,
								  const f32 windowCoordinateY1,
								  const f32 windowCoordinateX2,
								  const f32 windowCoordinateY2);

	public:
		/*!
			@brief Sets the size when rendering a font in 2D.

			However, because the original font data size is 8x8 pixels, a font size of 8 is recommended.

			@param[in] fontSize Specifies the font size in pixels ([1.0f,]).
		*/
		virtual void SetFontSize(const f32 fontSize);

		/*!
			@brief Draws text at a location specified in window coordinates.

			This function draws text by creating a rectangle using a TRIANGLE_STRIP, and then multiplying the resulting rasterized
			pixels by the font texture a pixel at a time.

			@param[in] windowCoordinateX Specifies the x-coordinate of the top-left of the rectangle in the window coordinate system where
		   the text is rendered (f32 range).
			@param[in] windowCoordinateY Specifies the y-coordinate of the top-left of the rectangle in the window coordinate system where
		   the text is rendered (f32 range).
			@param[in] format Specifies the format string.
		*/
		virtual void DrawText(const f32 windowCoordinateX, const f32 windowCoordinateY, const char* format, ...);

	public:
		/*!
			@brief  Checks whether the handle for the texture object is valid.

			@param[in] textureId Specifies the texture object handle.
			@return  Returns true if the texture object handle is valid; returns false otherwise.
		*/
		virtual bool HasTexture(const GLuint textureId);

		/*!
			@brief  Creates the texture object.

			Whatever is specified with the argument in this method is passed without modification to the glTexImage2D
			function.

			@param[in] target Texture type and allocated location.
			@param[in] internalFormat Specifies the format when the texture is saved.
			@param[in] width Specifies the width of the image in pixels.
			@param[in] height Specifies the height of the image in pixels.
			@param[in] format Format of the texture data to pass.
			@param[in] type Pixel data type.
			@param[in] pixels Pointer to image data.
			@param[out] textureId Texture object handle.
			@return  Returns true if the texture object was created successfully; returns false otherwise.
		*/
		virtual bool GenerateTexture(const GLenum  target,
									 const GLenum  internalFormat,
									 const GLsizei width,
									 const GLsizei height,
									 const GLenum  format,
									 const GLenum  type,
									 void*		   pixels,
									 GLuint&	   textureId);

		/*!
			@brief  Deletes the texture object specified by the texture objected handle.

			@param[in] textureId Specifies the texture object handle.
			@return Returns true if the texture object was deleted successfully; returns false otherwise.
		*/
		virtual bool DeleteTexture(const GLuint textureId);

	protected:
		virtual bool FindTextureArrayIndex(const GLuint textureId, u32& arrayIndex);

	public:
		/*!
			@brief  Uses the texture object specified by the handle to texture-map a rectangle specified in window coordinates and render it
		   in 2D.

			@param[in] textureId Specifies the texture object handle.
			@param[in] windowCoordinateX Specifies the x-coordinate of the top-left vertex of the rectangle in the window coordinate system
			(f32 range).
			@param[in] windowCoordinateY Specifies the y-coordinate of the top-left vertex of the rectangle in the window coordinate system
			(f32 range).
			@param[in] rectangleWidth Specifies the side in the x-direction of the rectangle in the window coordinate system (f32
			range).
			@param[in] rectangleHeight Specifies the side in the y-direction of the rectangle in the window coordinate system (f32
			range).
			@param[in] imageWidth Specifies the image width (f32<?tt> range).
			@param[in] imageHeight Specifies the image height (f32 range).
			@param[in] textureWidth Specifies the texture width (f32 range).
			@param[in] textureHeight Specifies the texture height (f32 range).
			@return Returns true if the texture object was deleted successfully; returns false otherwise.
		*/
		virtual void FillTexturedRectangle(const GLuint textureId,
										   const f32	windowCoordinateX,
										   const f32	windowCoordinateY,
										   const f32	rectangleWidth,
										   const f32	rectangleHeight,
										   const f32	imageWidth,
										   const f32	imageHeight,
										   const f32	textureWidth,
										   const f32	textureHeight);

		/*!
			@brief  Uses the texture object specified by the handle to draw a 2D triangle specified in window coordinates.

			Specify vertex data in counterclockwise order.

			@param[in] textureId Specifies the texture object handle.
			@param[in] windowCoordinateX0 Specifies the x-coordinate of the first vertex of the triangle in the window coordinate system
			(f32 range).
			@param[in] windowCoordinateY0 Specifies the y-coordinate of the first vertex of the triangle in the window coordinate system
			(f32 range).
			@param[in] texcoordS0 Specifies the s-coordinate of the first vertex of the triangle (f32 range).
			@param[in] texcoordT0 Specifies the t-coordinate of the first vertex of the triangle (f32 range).
			@param[in] windowCoordinateX1 Specifies the x-coordinate of the second vertex of the triangle in the window coordinate system
			(f32 range).
			@param[in] windowCoordinateY1 Specifies the y-coordinate of the second vertex of the triangle in the window coordinate system
			(f32 range).
			@param[in] texcoordS1 Specifies the s-coordinate of the second vertex of the triangle (f32 range).
			@param[in] texcoordT1 Specifies the t-coordinate of the second vertex of the triangle (f32 range).
			@param[in] windowCoordinateX2 Specifies the x-coordinate of the third vertex of the triangle in the window coordinate system
			(f32 range).
			@param[in] windowCoordinateY2 Specifies the y-coordinate of the third vertex of the triangle in the window coordinate system
			(f32 range).
			@param[in] texcoordS2 Specifies the s-coordinate of the third vertex of the triangle (f32 range).
			@param[in] texcoordT2 Specifies the t-coordinate of the third vertex of the triangle (f32 range).
		*/
		virtual void FillTexturedTriangle(const GLuint textureId,
										  const f32	   windowCoordinateX0,
										  const f32	   windowCoordinateY0,
										  const f32	   texcoordS0,
										  const f32	   texcoordT0,
										  const f32	   windowCoordinateX1,
										  const f32	   windowCoordinateY1,
										  const f32	   texcoordS1,
										  const f32	   texcoordT1,
										  const f32	   windowCoordinateX2,
										  const f32	   windowCoordinateY2,
										  const f32	   texcoordS2,
										  const f32	   texcoordT2);

	public:
		/*!
			@brief Displays the rendering result on the specified screen.
		*/
		virtual void SwapBuffers(void);

	protected:
		virtual void Flush(void);
		virtual void FlushTexturedPrimitives(void);

	protected:
		GraphicsDrawing m_GraphicsDrawing;

	protected:
		bool m_InitializeTexturedTriangles;
		bool m_UseDecalTextureProgram;
		bool m_Padding[2];

		u32	   m_MaxTexturedTrianglesNum;
		GLuint m_TextureIdArray[np::graphics::MAX_TEXTURES_NUM];

		u32								  m_TotalTexturedTrianglesNumArray[np::graphics::MAX_TEXTURES_NUM];
		np::graphics::TrianglesRenderData m_TexturedTrianglesRenderDataArray[np::graphics::MAX_TEXTURES_NUM];

		f32 m_ClearColorArray[3][4];
	};
}}	// namespace np::graphics

#endif
