#include <np/graphics.h>
#include <np/memory.h>

#include <nn/nstd.h>

#include <np/log_Macros.h>

namespace {
np::graphics::RenderSystemDrawing* s_RenderSystem;

uptr*					  s_RenderQueueHeap;
const u32				  s_RenderQueueHeapSize = 0x10000;
nn::os::SafeBlockingQueue s_RenderQueue;
bool					  s_RenderQueueInitialized = false;
nn::os::Thread			  s_RenderQueueThread;

uptr	  s_HeapForGx;
const u32 s_GxHeapSize = 0x800000;

int s_PrintfLineDisplay0 = 0;
int s_PrintfLineDisplay1 = 0;

char* s_PrintfBufferDisplay0[40];
char* s_PrintfBufferDisplay1[30];

void
nullCommand(np::graphics::RenderSystemDrawing* renderSystem, uptr param)
{
	NN_UNUSED_VAR(renderSystem);
	NN_UNUSED_VAR(param);
}

struct RenderCommand
{
	np::graphics::RenderCommandFunc command;
	uptr							param;
};

void
renderQueueLoop(uptr param)
{
	NN_UNUSED_VAR(param);

	NN_LOG_INFO("Render queue loop started");

	while (s_RenderQueueInitialized)
	{
		uptr		   command		 = s_RenderQueue.Dequeue();
		RenderCommand* renderCommand = reinterpret_cast<RenderCommand*>(command);

		NN_LOG_DEBUG("Executing render command");

		renderCommand->command(s_RenderSystem, renderCommand->param);

		NN_LOG_DEBUG("Cleaning up render command");

		delete renderCommand;
	}

	NN_LOG_INFO("Render queue loop exited");
}

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

struct PrintfData
{
	GLenum		display;
	const char* formattedText;
};

void
DrawPrintf(np::graphics::RenderSystemDrawing* renderSystem, GLenum display)
{
	renderSystem->SetRenderTarget(display);
	renderSystem->SetClearColor(display, GRAPHICS_COLOR_BLACK);
	renderSystem->Clear();
	renderSystem->SetColor(GRAPHICS_COLOR_WHITE);

	if (display == NN_GX_DISPLAY0)
		for (int i = 0; i < s_PrintfLineDisplay0; ++i)
		{
			if (!s_PrintfBufferDisplay0[i])
				continue;

			renderSystem->DrawText(0, LINE(i), s_PrintfBufferDisplay0[i]);
		}
	else
		for (int i = 0; i < s_PrintfLineDisplay1; ++i)
		{
			if (!s_PrintfBufferDisplay1[i])
				continue;

			renderSystem->DrawText(0, LINE(i), s_PrintfBufferDisplay1[i]);
		}

	renderSystem->SwapBuffers();
}

}  // namespace

namespace np { namespace graphics {

	Color COLOR_WHITE  = {GRAPHICS_COLOR_WHITE};
	Color COLOR_BLACK  = {GRAPHICS_COLOR_BLACK};
	Color COLOR_RED	   = {GRAPHICS_COLOR_RED};
	Color COLOR_GREEN  = {GRAPHICS_COLOR_GREEN};
	Color COLOR_BLUE   = {GRAPHICS_COLOR_BLUE};
	Color COLOR_ORANGE = {GRAPHICS_COLOR_ORANGE};

	void Initialize(void)
	{
		NN_LOG_INFO("Initializing np::graphics, heap size: 0x%08X", s_GxHeapSize);

		s_HeapForGx = reinterpret_cast<uptr>(np::memory::Allocate(s_GxHeapSize));

		s_RenderSystem = new np::graphics::RenderSystemDrawing();
		s_RenderSystem->Initialize(s_HeapForGx, s_GxHeapSize);
		s_RenderSystem->SetClearColor(NN_GX_DISPLAY1, GRAPHICS_COLOR_BLACK);
		s_RenderSystem->SetClearColor(NN_GX_DISPLAY0, GRAPHICS_COLOR_BLACK);
		s_RenderSystem->Clear();

		s_RenderSystem->SetRenderTarget(NN_GX_DISPLAY0);
		s_RenderSystem->SetColor(GRAPHICS_COLOR_WHITE);
		s_RenderSystem->SetFontSize(8);

		s_RenderQueueHeap = reinterpret_cast<uptr*>(np::memory::Allocate(s_RenderQueueHeapSize));
		NN_PANIC_IF_FAILED(s_RenderQueue.TryInitialize(s_RenderQueueHeap, s_RenderQueueHeapSize));

		s_RenderQueueInitialized = true;
		NN_PANIC_IF_FAILED(s_RenderQueueThread.TryStartUsingAutoStack(renderQueueLoop, NULL, 8 * 1024, 0));
	}

	void Finalize(void)
	{
		NN_LOG_INFO("Finalizing np::graphics");

		NN_LOG_INFO("Finalizing render queue, sending null command");

		s_RenderQueueInitialized = false;

		SubmitRenderCommand(nullCommand);
		s_RenderQueue.Finalize();
		np::memory::Free(reinterpret_cast<void*>(s_RenderQueueHeap));

		NN_LOG_INFO("Waiting for render queue thread to finish");

		s_RenderQueueThread.Join();
		s_RenderQueueThread.Finalize();

		s_RenderSystem->Finalize();
		np::memory::Free(reinterpret_cast<void*>(s_HeapForGx));

		delete s_RenderSystem;
	}

	void SubmitRenderCommand(RenderCommandFunc func, uptr param)
	{
		RenderCommand* command = new RenderCommand();
		command->command	   = func;
		command->param		   = param;

		s_RenderQueue.Enqueue(reinterpret_cast<uptr>(command));
	}

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

	void Printf(GLenum display, const char* text, ...)
	{
		// Handle overflow (remove the first line, shift all lines up, and add the new line)
		if (display == NN_GX_DISPLAY0)
		{
			if (s_PrintfLineDisplay0 + 1 > 40)
				for (int i = 0; i < s_PrintfLineDisplay0 - 1; ++i)
					s_PrintfBufferDisplay0[i] = s_PrintfBufferDisplay0[i + 1];
		}
		else if (s_PrintfLineDisplay1 + 1 > 30)
		{
			for (int i = 0; i < s_PrintfLineDisplay1 - 1; ++i)
				s_PrintfBufferDisplay1[i] = s_PrintfBufferDisplay1[i + 1];
		}

		// Increment the line count
		if (display == NN_GX_DISPLAY0 && s_PrintfLineDisplay0 < 40)
			s_PrintfLineDisplay0 += 1;
		else if (s_PrintfLineDisplay1 < 30)
			s_PrintfLineDisplay1 += 1;

		if (text == NULL)
			return;

		va_list args;
		va_start(args, text);

		char* buffer = new char[256];
		nn::nstd::TVSNPrintf(buffer, 256, text, args);

		va_end(args);

		// Append to the correct buffer
		if (display == NN_GX_DISPLAY0)
			s_PrintfBufferDisplay0[s_PrintfLineDisplay0 - 1] = buffer;
		else
			s_PrintfBufferDisplay1[s_PrintfLineDisplay1 - 1] = buffer;

		SubmitRenderCommand(::DrawPrintf, display);
	}

	void ClearScreen(GLenum display)
	{
		NN_LOG_DEBUG("Clearing screen, display: %d", display);

		if (display == NN_GX_DISPLAY0)
		{
			for (int i = 0; i < s_PrintfLineDisplay0; ++i)
				s_PrintfBufferDisplay0[i] = NULL;

			s_PrintfLineDisplay0 = 0;
		}
		else
		{
			for (int i = 0; i < s_PrintfLineDisplay1; ++i)
				s_PrintfBufferDisplay1[i] = NULL;

			s_PrintfLineDisplay1 = 0;
		}

		SubmitRenderCommand(::DrawPrintf, display);
	}

	np::graphics::RenderSystemDrawing* GetRenderSystem(void)
	{
		return s_RenderSystem;
	}

}}	// namespace np::graphics
