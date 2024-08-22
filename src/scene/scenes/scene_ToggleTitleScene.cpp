#include <np/scene/scene_SceneManager.h>
#include <np/scene/scenes/scene_ToggleTitleScene.h>

#include <np/api.h>
#include <np/config.h>

#include <np/log_Macros.h>

namespace {
bool s_ChangesMade = false;
}  // namespace

namespace np { namespace scene {

	ToggleTitleScene::ToggleTitleScene(void)
	{
		NN_LOG_INFO("Initializing np::scene::ToggleTitleScene");

		nn::cec::MessageBoxList messageBoxList;
		NN_PANIC_IF_FAILED(np::api::GetMessageBoxList(&messageBoxList));

		m_NumOptions		= messageBoxList.DirNum + 2;  // +1 for the return option
		m_Options			= new Option*[m_NumOptions];
		m_IndexToTitleIdMap = new nn::cec::TitleId[messageBoxList.DirNum];

		for (u32 i = 0; i < messageBoxList.DirNum; i++)
		{
			m_Options[i] = new Option();

			nn::cec::TitleId titleId = std::strtoul(reinterpret_cast<const char*>(messageBoxList.DirName[i]), NULL, 16);

			nn::cec::MessageBox messageBox;
			NN_PANIC_IF_FAILED(np::api::OpenMessageBox(titleId, &messageBox));

			size_t	titleLength = messageBox.GetMessageBoxDataSize(nn::cec::BOXDATA_TYPE_NAME_1);
			char16* title16		= reinterpret_cast<char16*>(std::malloc(titleLength));
			NN_PANIC_IF_FAILED(messageBox.GetMessageBoxData(nn::cec::BOXDATA_TYPE_NAME_1, title16, titleLength));

			char8* title = reinterpret_cast<char8*>(std::malloc(titleLength));
			std::wcstombs(title, title16, titleLength);

			m_Options[i]->text	   = title;
			m_Options[i]->callback = reinterpret_cast<OptionCallback>(&ToggleTitleScene::OnToggleTitleSelected);

			if (np::config::IsTitleIdIgnored(titleId))
				m_Options[i]->color = np::graphics::COLOR_RED;
			else
				m_Options[i]->color = np::graphics::COLOR_GREEN;

			m_IndexToTitleIdMap[i] = titleId;

			std::free(title16);
			messageBox.CloseMessageBox(true);
		}

		ADD_BLANK_OPTION(messageBoxList.DirNum);

		m_Options[messageBoxList.DirNum + 1]		   = new Option();
		m_Options[messageBoxList.DirNum + 1]->text	   = "Return";
		m_Options[messageBoxList.DirNum + 1]->callback = reinterpret_cast<OptionCallback>(&ToggleTitleScene::OnReturnToLastSceneSelected);
	}

	ToggleTitleScene::~ToggleTitleScene(void)
	{
		NN_LOG_INFO("Finalizing np::scene::ToggleTitleScene");

		for (u32 i = 0; i < m_NumOptions - 1; i++)
		{
			std::free(const_cast<char8*>(m_Options[i]->text));
			delete m_Options[i];
		}

		delete[] m_IndexToTitleIdMap;
		delete[] m_Options;
	}

	void ToggleTitleScene::OnToggleTitleSelected(void)
	{
		NN_LOG_INFO("Toggle title selected");

		ToggleTitleScene* toggleTitleScene = GetToggleTitleScene();

		Option* option = toggleTitleScene->m_Options[toggleTitleScene->m_SelectedOption];

		if (np::config::IsTitleIdIgnored(toggleTitleScene->m_IndexToTitleIdMap[toggleTitleScene->m_SelectedOption]))
		{
			option->color = np::graphics::COLOR_GREEN;
			np::config::RemoveIgnoredTitle(toggleTitleScene->m_IndexToTitleIdMap[toggleTitleScene->m_SelectedOption]);
		}
		else
		{
			option->color = np::graphics::COLOR_RED;
			np::config::AddIgnoredTitle(toggleTitleScene->m_IndexToTitleIdMap[toggleTitleScene->m_SelectedOption]);
		}

		s_ChangesMade = true;

		toggleTitleScene->Render();
	}

	void ToggleTitleScene::OnReturnToLastSceneSelected(void)
	{
		if (s_ChangesMade)
		{
			PRINTF_DISPLAY1("!!! WARNING !!!");
			PRINTF_DISPLAY1("Any changes made will not be committed");
			PRINTF_DISPLAY1("until the application is closed");
			PRINTF_DISPLAY1("(via start or home menu)");
			PRINTF_DISPLAY1("!!! WARNING !!!");

			s_ChangesMade = false;
		}

		ChangeScene(GetSettingsScene());
	}

	void ToggleTitleScene::Step(const nn::hid::PadStatus& padStatus)
	{
		if (padStatus.trigger & nn::hid::BUTTON_B)
		{
			OnReturnToLastSceneSelected();

			return;
		}
		OptionSceneBase::Step(padStatus);
	}

	void ToggleTitleScene::DrawOptions(np::graphics::RenderSystemDrawing* renderSystem)
	{
		renderSystem->SetRenderTarget(NN_GX_DISPLAY0);
		renderSystem->Clear();

		renderSystem->SetColor(GRAPHICS_COLOR_WHITE);

		// Title bar
		renderSystem->DrawText(0, 0, "Settings");
		renderSystem->DrawText(0, LINE(1), "Press (A) on an option to change it");

		m_OptionsOffset = 2;

		OptionSceneBase::DrawOptions(renderSystem, false);
	}

}}	// namespace np::scene
