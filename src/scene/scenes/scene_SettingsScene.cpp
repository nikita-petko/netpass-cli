#include <np/scene/scene_SceneManager.h>
#include <np/scene/scenes/scene_SettingsScene.h>

#include <np/config.h>

#include <np/log.h>

namespace {
np::scene::SceneBase* s_LastScene;
bool				  s_ChangesMade = false;
}  // namespace

namespace np { namespace scene {

	SettingsScene::SettingsScene(void)
	{
		NN_LOG_INFO("Initializing np::scene::SettingsScene");

		m_NumOptions = 3;
		m_Options	 = new Option*[m_NumOptions];

		m_Options[0] = new Option();
		m_Options[1] = new Option();
		m_Options[2] = new Option();

		m_Options[0]->text = "Toggle titles";
		m_Options[1]->text = "Toggle show help";
		m_Options[2]->text = "Return";

		m_Options[0]->callback = reinterpret_cast<OptionCallback>(&SettingsScene::OnToggleTitlesSelected);
		m_Options[1]->callback = reinterpret_cast<OptionCallback>(&SettingsScene::OnToggleShowHelpSelected);
		m_Options[2]->callback = reinterpret_cast<OptionCallback>(&SettingsScene::OnReturnToLastSceneSelected);

		if (np::config::GetConfiguration()->show_help)
			m_Options[1]->color = np::graphics::COLOR_GREEN;
		else
			m_Options[1]->color = np::graphics::COLOR_RED;
	}

	SettingsScene::~SettingsScene(void)
	{
		NN_LOG_INFO("Finalizing np::scene::SettingsScene");

		for (u32 i = 0; i < m_NumOptions; i++)
			delete m_Options[i];

		delete[] m_Options;
	}

	void SettingsScene::OnToggleTitlesSelected(void)
	{
		NN_LOG_INFO("Toggle titles selected");

		ChangeScene(GetToggleTitleScene());
	}

	void SettingsScene::OnToggleShowHelpSelected(void)
	{
		np::config::Configuration* config = np::config::GetConfiguration();

		config->show_help = !config->show_help;

		SettingsScene* settingsScene = GetSettingsScene();

		if (config->show_help)
			settingsScene->m_Options[1]->color = np::graphics::COLOR_GREEN;
		else
			settingsScene->m_Options[1]->color = np::graphics::COLOR_RED;

		s_ChangesMade = true;

		settingsScene->Render();
	}

	void SettingsScene::OnReturnToLastSceneSelected(void)
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

		ChangeScene(s_LastScene);

		s_LastScene = NULL;
	}

	void SettingsScene::SetLastScene(SceneBase* lastScene)
	{
		s_LastScene = lastScene;
	}

	void SettingsScene::Step(const nn::hid::PadStatus& padStatus)
	{
		if (padStatus.trigger & nn::hid::BUTTON_B)
		{
			OnReturnToLastSceneSelected();

			return;
		}
		OptionSceneBase::Step(padStatus);
	}

	void SettingsScene::DrawOptions(np::graphics::RenderSystemDrawing* renderSystem)
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
