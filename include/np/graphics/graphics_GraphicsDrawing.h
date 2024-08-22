#ifndef NP_GRAPHICS_GRAPHICS_GRAPHICS_DRAWING_H_
#define NP_GRAPHICS_GRAPHICS_GRAPHICS_DRAWING_H_

#include <nn/gx.h>
#include <nn/util/util_NonCopyable.h>

#include <np/graphics/graphics_TextsRenderData.h>
#include <np/graphics/graphics_TrianglesRenderData.h>

#include <np/graphics/graphics_ShaderManager.h>

namespace np { namespace graphics {
	/*!
		@brief  This class performs 2D drawing operations on the CTR.
	*/
	class GraphicsDrawing : private nn::util::NonCopyable<GraphicsDrawing>
	{
	public:
		GraphicsDrawing(void);
		virtual ~GraphicsDrawing(void);

	public:
		virtual void Initialize(void);
		virtual void Finalize(void);

	public:
		/*!
			@brief Initializes 2D triangles for drawing.

			This function does not need to be called explicitly because it is implicitly called by the DrawTriangle2d
			function. Only call this function if you want to change the maximum number of 2D triangles to draw on the upper and lower LCD
			screens. Call it immediately after calling the Initialize function.

			@param[in] maxTrianglesNum Specifies the maximum number of triangles to render on each LCD, both upper and lower. (The default
		   is MAX_TRIANGLES_NUM.)
		*/
		virtual void InitializeTriangles(const u32 maxTrianglesNum = graphics::MAX_TRIANGLES_NUM);

		/*!
			@brief Initializes 2D quadrangles for drawing.

			This function does not need to be explicitly called because it is implicitly called inside the DrawSquare2d
			function. Only call this function if you want to change the maximum number of 2D quadrangles (including points and lines) to
		   draw on the upper and lower LCD screens. Call it immediately after calling the Initialize function.

			@param[in] maxSquaresNum Specifies the maximum number of quadrangles to render on each LCD, both upper and lower. (The default
		   is MAX_SQUARES_NUM.)
		*/
		virtual void InitializeSquares(const u32 maxSquaresNum = graphics::MAX_SQUARES_NUM);

		/*!
			@brief Initializes 2D text for drawing.

			This function does not need to be explicitly called because it is implicitly called inside the DrawText2d
			function. Only call this function if you want to change the maximum number of 2D text characters to draw on the upper and lower
		   LCD screens. Call it immediately after calling the Initialize function.

			@param[in] maxLength Specifies the maximum number of characters to render on each LCD, both upper and lower. (The default is
		   MAX_TEXT_NUM.)
		*/
		virtual void InitializeTexts(const u32 maxLength = graphics::MAX_TEXT_LENGTH);

	public:
		/*!
			@brief Sets the window size for 2D drawing.

			@param[in] windowWidth Specifies the window width.
			@param[in] windowWidth Specifies the window height.
		*/
		virtual void SetWindowSize(const f32 windowWidth, const f32 windowHeight);

		/*!
			@brief Gets the window size for 2D drawing.

			@param[out] windowWidth Specifies the window width.
			@param[out] windowHeight Specifies the window height.
		*/
		virtual void GetWindowSize(f32& windowWidth, f32& windowHeight);

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

		/*!
			@brief Reduces the depth when rendering primitives in 2D.
		*/
		virtual void DecrementDepth(void);

	public:
		/*!
			@brief Sets the size when drawing a point in 2D.

			@param[in] pointSize Specifies the point size in pixels ([1.0f,]).
		*/
		virtual void SetPointSize(const f32 pointSize);

		/*!
			@brief Draws a 2D point specified in window coordinates.

			@param[in] windowCoordinateX Specifies the x-coordinate of the center of the point in the window coordinate system (f32 range).
			@param[in] windowCoordinateY Specifies the y-coordinate of the center of the point in the window coordinate system (f32 range).
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
		   system (f32 range).
			@param[in] windowCoordinateY1 Specifies the y-coordinate of the bottom-left vertex of the quadrangle in the window coordinate
		   system (f32 range).
			@param[in] windowCoordinateX2 Specifies the x-coordinate of the bottom-right vertex of the quadrangle in the window coordinate
		   system (f32 range).
			@param[in] windowCoordinateY2 Specifies the y-coordinate of the bottom-right vertex of the quadrangle in the window coordinate
		   system (f32 range).
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

		/*!
		@brief Draws text at a location specified in window coordinates.

		This function draws text by creating a rectangle using a TRIANGLE_STRIP, and then multiplying the resulting rasterized
		pixels by the font texture a pixel at a time.

		@param[in] windowCoordinateX Specifies the x-coordinate of the top-left of the rectangle in the window coordinate system where the
		text is rendered (f32 range).
		@param[in] windowCoordinateY Specifies the y-coordinate of the top-left of the rectangle in the window coordinate system where the
		text is rendered (f32 range).
		@param[in] text Specifies the string.
		*/
		virtual void DrawTextBuffer(const f32 windowCoordinateX, const f32 windowCoordinateY, const char* textBuffer);

	public:
		/*!
			@brief Displays the rendering result on the specified screen.
		*/
		virtual void Flush(void);

	protected:
		virtual void FlushColorFillPrimitives(void);
		virtual void FlushTexts(void);

	public:
		f32	 m_Color[4];
		f32	 m_Depth;
		bool m_AutoDepth;
		bool m_Padding0[3];
		f32	 m_DepthValue;

		f32 m_WindowWidth;
		f32 m_WindowHeight;

	public:
		graphics::ShaderManager m_ShaderManager;

	protected:
		bool m_InitializeTriangles;
		bool m_InitializeSquares;
		bool m_InitializeTexts;
		bool m_Padding1;

	protected:
		u32 m_MaxTrianglesNum;
		u32 m_TotalTrianglesNum;

	protected:
		u32 m_MaxSquaresNum;
		u32 m_TotalSquaresNum;

	protected:
		u32 m_MaxTextLength;
		u32 m_TotalTextLength;

	protected:
		bool m_UseColorFillProgram;
		bool m_UseFontProgram;
		bool m_Padding2[2];

	protected:
		f32 m_PointSize;
		f32 m_LineWidth;

		graphics::TrianglesRenderData m_TrianglesRenderData;
		graphics::TrianglesRenderData m_SquaresRenderData;

		f32						  m_FontSize;
		graphics::TextsRenderData m_TextsRenderData;
	};
}}	// namespace np::graphics

#endif
