#include <np/graphics.h>

namespace {

struct StatusScreenData
{
	const f32*	color;
	const char* text;
	const f32*	textColor;
};

void
DrawStatusScreen(np::graphics::RenderSystemDrawing* renderSystem, StatusScreenData* param)
{
	const f32*	color	  = param->color;
	const char* text	  = param->text;
	const f32*	textColor = param->textColor;

	renderSystem->SetRenderTarget(NN_GX_DISPLAY0);

	renderSystem->SetClearColor(NN_GX_DISPLAY0, color[0], color[1], color[2], color[3]);
	renderSystem->Clear();

	renderSystem->SetColor(textColor[0], textColor[1], textColor[2], textColor[3]);
	np::graphics::DrawCenteredText(renderSystem, text);

	renderSystem->SwapBuffers();

	delete param;
}

}  // namespace

namespace np { namespace graphics {

	void DrawCenteredText(np::graphics::RenderSystemDrawing* renderSystem, const char* text, ...)
	{
		u32 windowSizeY = renderSystem->m_CurrentDisplayBuffersPtr->GetWidth();
		u32 windowSizeX = renderSystem->m_CurrentDisplayBuffersPtr->GetHeight();

		NN_LOG_INFO("Screen size: %d x %d", windowSizeX, windowSizeY);

		const u32 textSizePixels = CHARS(std::strlen(text) + 1);

		NN_LOG_INFO("Text size: %d", textSizePixels);

		// Centered text is when there are equal amounts of pixels on both sides of the text
		// As well as vertically

		const u32 textX = (windowSizeX - textSizePixels) / 2;
		const u32 textY = (windowSizeY - FONT_SIZE) / 2;

		NN_LOG_INFO("Text position: %d, %d", textX, textY);

		renderSystem->DrawText(textX, textY, text);
		renderSystem->SwapBuffers();
	}

	void DrawStatusScreen(const Color color, const char* text, const Color textColor)
	{
		NN_LOG_DEBUG("Drawing status screen, text: %s", text);

		StatusScreenData* data = new StatusScreenData();
		data->color			   = color;
		data->text			   = text;
		data->textColor		   = textColor;

		SubmitRenderCommand(::DrawStatusScreen, data);
	}

	void DrawFatality(const char* text)
	{
		NN_LOG_DEBUG("Drawing fatality screen, text: %s", text);

		DrawStatusScreen(COLOR_RED, text);
	}

}}	// namespace np::graphics
