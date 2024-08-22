#include <np/graphics.h>

#include <nn/nstd.h>

#include <np/log.h>

namespace {

int s_ConsoleLineDisplay0 = 0;
int s_ConsoleLineDisplay1 = 0;

char* s_ConsoleBufferDisplay0[40];	// 40 lines of text
char* s_ConsoleBufferDisplay1[30];	// 30 lines of text

void
DrawConsole(np::graphics::RenderSystemDrawing* renderSystem, GLenum display)
{
	renderSystem->SetRenderTarget(display);
	renderSystem->SetClearColor(display, GRAPHICS_COLOR_BLACK);
	renderSystem->Clear();
	renderSystem->SetColor(GRAPHICS_COLOR_WHITE);

	if (display == NN_GX_DISPLAY0)
		for (int i = 0; i < s_ConsoleLineDisplay0; ++i)
		{
			if (!s_ConsoleBufferDisplay0[i])
				continue;

			renderSystem->DrawText(0, LINE(i), s_ConsoleBufferDisplay0[i]);
		}
	else
		for (int i = 0; i < s_ConsoleLineDisplay1; ++i)
		{
			if (!s_ConsoleBufferDisplay1[i])
				continue;

			renderSystem->DrawText(0, LINE(i), s_ConsoleBufferDisplay1[i]);
		}

	renderSystem->SwapBuffers();
}

}  // namespace

namespace np { namespace graphics {

	void Printf(GLenum display, const char* text, ...)
	{
		// Handle overflow (remove the first line, shift all lines up, and add the new line)
		if (display == NN_GX_DISPLAY0)
		{
			if (s_ConsoleLineDisplay0 + 1 > 40)
				for (int i = 0; i < s_ConsoleLineDisplay0 - 1; ++i)
					s_ConsoleBufferDisplay0[i] = s_ConsoleBufferDisplay0[i + 1];
		}
		else if (s_ConsoleLineDisplay1 + 1 > 30)
		{
			for (int i = 0; i < s_ConsoleLineDisplay1 - 1; ++i)
				s_ConsoleBufferDisplay1[i] = s_ConsoleBufferDisplay1[i + 1];
		}

		// Increment the line count
		if (display == NN_GX_DISPLAY0 && s_ConsoleLineDisplay0 < 40)
			s_ConsoleLineDisplay0 += 1;
		else if (s_ConsoleLineDisplay1 < 30)
			s_ConsoleLineDisplay1 += 1;

		if (text == NULL)
			return;

		va_list args;
		va_start(args, text);

		char* buffer = new char[256];
		nn::nstd::TVSNPrintf(buffer, 256, text, args);

		va_end(args);

		// Append to the correct buffer
		if (display == NN_GX_DISPLAY0)
			s_ConsoleBufferDisplay0[s_ConsoleLineDisplay0 - 1] = buffer;
		else
			s_ConsoleBufferDisplay1[s_ConsoleLineDisplay1 - 1] = buffer;

		SubmitRenderCommand(::DrawConsole, display);
	}

	void ClearScreen(GLenum display)
	{
		NN_LOG_DEBUG("Clearing screen, display: %d", display);

		if (display == NN_GX_DISPLAY0)
		{
			for (int i = 0; i < s_ConsoleLineDisplay0; ++i)
				s_ConsoleBufferDisplay0[i] = NULL;

			s_ConsoleLineDisplay0 = 0;
		}
		else
		{
			for (int i = 0; i < s_ConsoleLineDisplay1; ++i)
				s_ConsoleBufferDisplay1[i] = NULL;

			s_ConsoleLineDisplay1 = 0;
		}

		SubmitRenderCommand(::DrawConsole, display);
	}

}}	// namespace np::graphics
