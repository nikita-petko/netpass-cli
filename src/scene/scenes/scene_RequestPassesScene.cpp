#include <np/scene/scene_SceneManager.h>
#include <np/scene/scenes/scene_RequestPassesScene.h>

#include <np/api.h>
#include <np/config.h>
#include <np/memory.h>
#include <np/util.h>

#include <nn/applet.h>
#include <nn/http.h>
#include <nn/nstd.h>
#include <nn/swkbd.h>

#include <np/log.h>

namespace {

u32				 s_Passes = 10;
nn::cec::TitleId s_TitleId;

np::scene::LoadingScene::LoadingCallbackResult
LoadingCallback()
{
	PRINTF_DISPLAY1("Requesting %d passes", s_Passes);

	for (u32 i = 0; i < s_Passes; i++)
	{
		nn::Result result = np::api::RequestPass(s_TitleId);

		if (result.IsFailure())
		{
			if (result == nn::http::ResultTimeout())
				return np::scene::LoadingScene::LOADING_CALLBACK_RESULT_RETRY;

			NN_LOG_ERROR("Failed to request pass:");
			NN_DBG_PRINT_RESULT(result);

			np::graphics::DrawFatality("ERROR REQUESTING PASS");
			PRINTF_DISPLAY1("Failed to request pass");

			return np::scene::LoadingScene::LOADING_CALLBACK_RESULT_ERROR;
		}
	}

	s_Passes  = 1;
	s_TitleId = 0;

	np::api::TriggerInboxDownload();
	np::api::WaitForInboxDownloadComplete();

	return np::scene::LoadingScene::LOADING_CALLBACK_RESULT_NEXT_SCENE;
}

}  // namespace

namespace np { namespace scene {

	RequestPassesScene::RequestPassesScene(void)
	{
		NN_LOG_INFO("Initializing np::scene::RequestPassesScene");

		nn::cec::MessageBoxList messageBoxList;
		NN_PANIC_IF_FAILED(np::util::GetCecMessageBoxList(&messageBoxList));

		np::config::ClearIgnoredTitles(&messageBoxList);

		m_CriticalSection.Initialize();

		m_NumOptions		  = messageBoxList.DirNum + 2;
		m_Options			  = new Option*[m_NumOptions];
		m_IndexToTitleIdMap	  = new nn::cec::TitleId[messageBoxList.DirNum];
		m_IndexToTitleNameMap = new char8*[messageBoxList.DirNum];

		for (u32 i = 0; i < messageBoxList.DirNum; i++)
		{
			m_Options[i] = new Option();

			nn::cec::TitleId titleId = std::strtoul(reinterpret_cast<const char*>(messageBoxList.DirName[i]), NULL, 16);

			size_t titleNameSize;
			NN_PANIC_IF_FAILED(np::util::GetTitleName(titleId, NULL, &titleNameSize));

			char16* title16 = reinterpret_cast<char16*>(std::malloc(titleNameSize));
			NN_PANIC_IF_FAILED(np::util::GetTitleName(titleId, title16, &titleNameSize));

			char8* title = reinterpret_cast<char8*>(std::malloc(titleNameSize));
			std::wcstombs(title, title16, titleNameSize);

			// If the title cannot be converted to a string (i.e. it contains non-ASCII characters), use the title ID instead
			if (*title == NULL)
			{
				std::free(title);

				title = reinterpret_cast<char8*>(std::malloc(sizeof(messageBoxList.DirName[i])));
				std::memcpy(title, messageBoxList.DirName[i], sizeof(messageBoxList.DirName[i]));
			}

			nn::cec::CecBoxInfoHeader boxInfo;
			NN_PANIC_IF_FAILED(np::util::GetCecMessageBoxHeader(titleId, nn::cec::CEC_BOXTYPE_INBOX, &boxInfo));

			u32 messageCount = boxInfo.messNum;
			u32 maxMessages	 = boxInfo.messNumMax;

			// Allocate a buffer to account for "({count}/{max}) " + title
			char8* titleWithCount = reinterpret_cast<char8*>(std::malloc(titleNameSize + 16));
			nn::nstd::TSNPrintf(titleWithCount, titleNameSize + 16, "(%d/%d) %s", messageCount, maxMessages, title);

			m_Options[i]->text	   = titleWithCount;
			m_Options[i]->callback = reinterpret_cast<OptionCallback>(&RequestPassesScene::OnTitleSelected);

			if (messageCount == maxMessages)
				m_Options[i]->color = np::graphics::COLOR_RED;

			m_IndexToTitleIdMap[i]	 = titleId;
			m_IndexToTitleNameMap[i] = title;

			std::free(title16);
		}

		ADD_BLANK_OPTION(messageBoxList.DirNum);

		m_Options[messageBoxList.DirNum + 1]		   = new Option();
		m_Options[messageBoxList.DirNum + 1]->text	   = "Return";
		m_Options[messageBoxList.DirNum + 1]->callback = reinterpret_cast<OptionCallback>(&RequestPassesScene::OnReturnToLastSceneSelected);
	}

	RequestPassesScene::~RequestPassesScene(void)
	{
		NN_LOG_INFO("Finalizing np::scene::RequestPassesScene");

		for (u32 i = 0; i < m_NumOptions - 1; i++)
		{
			std::free(const_cast<char8*>(m_Options[i]->text));
			// std::free(const_cast<char8*>(m_IndexToTitleNameMap[i]));

			delete m_Options[i];
		}

		m_CriticalSection.Finalize();

		delete[] m_IndexToTitleIdMap;
		delete[] m_IndexToTitleNameMap;

		delete[] m_Options;
	}

	void RequestPassesScene::OnTitleSelected(void)
	{
		NN_LOG_INFO("Title selected");

		nn::Result result;

		RequestPassesScene* requestPassesScene = GetRequestPassesScene();
		nn::cec::TitleId	titleId			   = requestPassesScene->m_IndexToTitleIdMap[requestPassesScene->m_SelectedOption];

		nn::cec::CecBoxInfoHeader boxInfo;
		result = np::util::GetCecMessageBoxHeader(titleId, nn::cec::CEC_BOXTYPE_INBOX, &boxInfo);
		if (result.IsFailure())
		{
			NN_LOG_ERROR("Failed to get message box header:");
			NN_DBG_PRINT_RESULT(result);

			np::graphics::DrawFatality("ERROR GETTING MESSAGE BOX HEADER");
			PRINTF_DISPLAY1("Failed to get message box header");

			return;
		}

		u32 maxMessages	 = boxInfo.messNumMax;
		u32 messageCount = boxInfo.messNum;

		if (messageCount == maxMessages)
		{
			PRINTF_DISPLAY1("The inbox for this title is full");

			return;
		}

		nn::applet::WakeupState wstate;
		nn::swkbd::Parameter	kbd_param;

		nn::swkbd::InitializeConfig(&kbd_param.config);
		kbd_param.config.keyboardType												  = nn::swkbd::KEYBOARD_TYPE_TENKEY;
		kbd_param.config.upperScreenFlag											  = nn::swkbd::UPPER_SCREEN_DARK;
		kbd_param.config.bottomButtonType											  = nn::swkbd::BOTTOM_BUTTON_TYPE_2BUTTON;
		kbd_param.config.textLengthMax												  = 2;
		kbd_param.config.prediction													  = false;
		kbd_param.config.lineFeed													  = false;
		kbd_param.config.bottomButtonToFinish[nn::swkbd::BOTTOM_BUTTON_2BUTTON_RIGHT] = true;

		s32	  memSize	= nn::swkbd::GetSharedMemorySize(&kbd_param.config, NULL, NULL);
		void* sharedBuf = std::malloc(memSize + (nn::swkbd::MEMORY_ALIGNMENT - 1));
		if (!sharedBuf)
		{
			PRINTF_DISPLAY1("Failed to allocate memory for keyboard");

			return;
		}

		void* alignedSharedBuf = reinterpret_cast<void*>(MEM_ROUNDUP(reinterpret_cast<u32>(sharedBuf), nn::swkbd::MEMORY_ALIGNMENT));
		nn::swkbd::StartKeyboardApplet(&wstate, &kbd_param, alignedSharedBuf, memSize, L"1");

		// Recover the GPU register settings
		if (nn::applet::IsExpectedToCloseApplication())
		{
			std::free(sharedBuf);

			Exit();

			return;
		}

		nngxUpdateState(NN_GX_STATE_ALL);
		nngxValidateState(NN_GX_STATE_ALL, GL_TRUE);

		if (kbd_param.config.returnCode == nn::swkbd::RETURN_CODE_BOTTOM_BUTTON_2BUTTON_LEFT)
		{
			std::free(sharedBuf);

			return;
		}

		// Get the input string
		u8*		buf = static_cast<u8*>(alignedSharedBuf);
		wchar_t text_buf[3];
		std::memset(text_buf, 0, sizeof(text_buf));

		std::memcpy(text_buf, &buf[kbd_param.config.inputText], kbd_param.config.inputTextLength * sizeof(wchar_t));

		char8* text = new char8[3];
		std::wcstombs(text, text_buf, 3);

		char8* p;
		s_Passes = std::strtoul(text, &p, 10);
		if (*p)
		{
			PRINTF_DISPLAY1("The input is not a number");

			std::free(text);
			std::free(sharedBuf);

			return;
		}

		std::free(text);
		std::free(sharedBuf);

		if (s_Passes < 1 || s_Passes > maxMessages)
		{
			PRINTF_DISPLAY1("The input is not between 1 and %d", maxMessages);

			return;
		}

		if (messageCount + s_Passes > maxMessages)
		{
			PRINTF_DISPLAY1("The inbox will be full after this.");
			PRINTF_DISPLAY1("Changing to %d", maxMessages - messageCount);

			s_Passes = maxMessages - messageCount;
		}

		s_TitleId = titleId;

		LoadingScene* loadingScene = GetLoadingScene();
		loadingScene->Update(requestPassesScene, reinterpret_cast<LoadingScene::LoadingCallback>(&LoadingCallback));

		ChangeScene(loadingScene);
	}

	void RequestPassesScene::UpdateTitleText(nn::cec::TitleId titleId, u32 boxMessages, u32 maxBoxMessages)
	{
		NN_LOG_INFO("Updating title text for title ID %08X", titleId);

		m_CriticalSection.Enter();

		for (u32 i = 0; i < m_NumOptions - 1; i++)
		{
			if (m_IndexToTitleIdMap[i] == titleId)
			{
				Option* option = m_Options[i];

				std::free(const_cast<char8*>(option->text));

				char8* titleWithCount = reinterpret_cast<char8*>(std::malloc(std::strlen(m_IndexToTitleNameMap[i]) + 16));
				nn::nstd::TSNPrintf(titleWithCount,
									std::strlen(m_IndexToTitleNameMap[i]) + 16,
									"(%d/%d) %s",
									boxMessages,
									maxBoxMessages,
									m_IndexToTitleNameMap[i]);

				option->text = titleWithCount;

				if (boxMessages == maxBoxMessages)
					option->color = np::graphics::COLOR_RED;

				break;
			}
		}

		m_CriticalSection.Leave();

		NN_LOG_INFO("Checking if the scene has to be redrawn");

		if (IsActive(this))
		{
			NN_LOG_INFO("Redrawing scene");

			Render();
		}
	}

	void RequestPassesScene::OnReturnToLastSceneSelected(void)
	{
		ChangeScene(GetLocationScene());
	}

	void RequestPassesScene::Step(const nn::hid::PadStatus& padStatus)
	{
		if (padStatus.trigger & nn::hid::BUTTON_B)
		{
			OnReturnToLastSceneSelected();

			return;
		}
		OptionSceneBase::Step(padStatus);
	}

	void RequestPassesScene::DrawOptions(np::graphics::RenderSystemDrawing* renderSystem)
	{
		m_CriticalSection.Enter();

		renderSystem->SetRenderTarget(NN_GX_DISPLAY0);
		renderSystem->Clear();

		renderSystem->SetColor(GRAPHICS_COLOR_WHITE);

		// Title bar
		renderSystem->DrawText(0, 0, "Request passes");
		renderSystem->DrawText(0, LINE(1), "Press (A) to prompt for passes");

		m_OptionsOffset = 2;

		OptionSceneBase::DrawOptions(renderSystem, false);

		m_CriticalSection.Leave();
	}

}}	// namespace np::scene
