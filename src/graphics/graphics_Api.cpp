#include <np/graphics.h>
#include <np/memory.h>

#include <np/log.h>

namespace {
np::graphics::RenderSystemDrawing* s_RenderSystem;

uptr*					  s_RenderQueueHeap;
const u32				  s_RenderQueueHeapSize = 0x10000;
nn::os::SafeBlockingQueue s_RenderQueue;
bool					  s_RenderQueueInitialized = false;
nn::os::Thread			  s_RenderQueueThread;

uptr	  s_HeapForGx;
const u32 s_GxHeapSize = 0x800000;

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
}  // namespace

namespace np { namespace graphics {

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

}}	// namespace np::graphics
