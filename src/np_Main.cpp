#include <nn.h>
#include <nn/dbg.h>
#include <nn/types.h>

#include <nn/fnd/fnd_ExpHeap.h>
#include <nn/os/os_Memory.h>
#include <nn/os/os_MemoryTypes.h>

#include <nn/fs.h>
#include <nn/gx.h>

#include <nn/applet.h>
#include <nn/applet/applet_Private.h>

#include <wchar.h>
#include <cstring>

#include <np/api.h>
#include <np/config.h>
#include <np/graphics.h>
#include <np/http.h>
#include <np/memory.h>

#include <np/scene/scene_SceneManager.h>

#include <np/log.h>
#include <np/log_Macros.h>

/*
		@brief  The allocator to be used with cecd.
	*/
class CecAllocator : public nn::fnd::IAllocator
{
public:
	virtual void* Allocate(size_t size, s32 alignment = 4) { return np::memory::Allocate(size, alignment); }
	virtual void  Free(void* ptr) { np::memory::Free(ptr); }
};

namespace {
CecAllocator s_CecAllocator;
}  // namespace

void
onRecvAwakeNotification(uptr arg)
{
	NN_UNUSED_VAR(arg);

	if (nn::applet::IsActive())
		nngxStartLcdDisplay();
}

nn::applet::QueryReply
defaultReplyFunc(uptr arg)
{
	NN_UNUSED_VAR(arg);
	return nn::applet::REPLY_REJECT;
}

void
Finalize(void)
{
	NN_LOG_INFO("Finalizing np::scene, np::config, np::http, np::memory, np::graphics");

	np::api::Finalize();
	np::config::Finalize();
	np::scene::Finalize();
	np::graphics::Finalize();
	np::memory::Finalize();
	np::http::Finalize();

	NN_LOG_INFO("Finalizing nn::cec, nn::hid");

	nn::cec::Finalize();
	nn::hid::Finalize();

	NN_LOG_INFO("Finalizing np::log");

#ifdef NP_LOGGING_ENABLED
	np::log::Finalize();
#endif
}

void
Initialize(void)
{
	nn::fs::Initialize();

#ifdef NP_LOGGING_ENABLED
	NN_LOG_NAMESPACE(true);
	NN_LOG_SIGNATURE(false);
	NN_LOG_LONGPATH(false);
	NN_LOG_SET_LOG_LEVEL(NN_LOG_LEVEL);
	np::log::Initialize();
#endif

	NN_LOG_INFO("Initializing nn::applet");

	nn::applet::SetSleepQueryCallback(defaultReplyFunc);
	nn::applet::SetAwakeCallback(onRecvAwakeNotification);

	nn::applet::Enable();

	if (nn::applet::IsExpectedToCloseApplication())
	{
		NN_LOG_WARN("Early exit requested, closing application");

		// Quickly end the application
		nn::applet::CloseApplication();

		// Never reached
		return;
	}

	NN_LOG_INFO("Initializing np::memory");

	np::memory::Initialize();

	NN_LOG_INFO("Initializing graphics");

	np::graphics::Initialize();

	NN_LOG_INFO("Initializing nn::hid");
	NN_PANIC_IF_FAILED(nn::hid::Initialize());

	NN_LOG_INFO("Initializing nn::cec");
	NN_PANIC_IF_FAILED(nn::cec::Initialize(s_CecAllocator));

	NN_LOG_INFO("Initializing np::http, np::config, np::scene");
	np::http::Initialize();
	np::config::Initialize();
	np::scene::Initialize();
}

void
validateGx()
{
	// Validate GX state
	nngxUpdateState(NN_GX_STATE_ALL);
	nngxValidateState(NN_GX_STATE_ALL, GL_TRUE);
}

extern "C" void
nnMain()
{
	Initialize();

	nn::hid::PadReader padReader;
	nn::hid::PadStatus ps;

	if (np::config::GetConfiguration().show_help)
		np::scene::ChangeScene(np::scene::GetHelpScene());
	else
		np::scene::GetHelpScene()->NextScene();

	while (1)
	{
		padReader.ReadLatest(&ps);

		if (ps.trigger & nn::hid::BUTTON_START)
			break;

		if (!np::scene::Step(ps))
			break;

		if (nn::applet::IsExpectedToProcessHomeButton())
		{
			nn::applet::ProcessHomeButtonAndWait();

			if (nn::applet::IsExpectedToCloseApplication())
				break;

			validateGx();
		}

		if (nn::applet::IsExpectedToProcessPowerButton())
		{
			nn::applet::ProcessPowerButtonAndWait();

			if (nn::applet::IsExpectedToCloseApplication())
				break;

			validateGx();
		}

		if (nn::applet::IsExpectedToCloseApplication())
			break;
	}

	Finalize();

	// Close application (regular as IsExpectedToCloseApplication() should be true)
	nn::applet::CTR::detail::CloseApplicationCore();
}
