#include <np/scene/scene_SceneManager.h>
#include <np/scene/scenes/scene_LocationScene.h>

#include <np/api.h>
#include <np/graphics.h>

#include <np/log_Macros.h>

namespace {

const char*
CurrentLocationToString()
{
	switch (np::api::s_Location)
	{
		case 0:
			return "Train Station";
		case 1:
			return "Plaza";
		case 2:
			return "Mall";
		case 3:
			return "Beach";
		case 4:
			return "Arcade";
		case 5:
			return "Cat Cafe";
		default:
			return "Home";
	}
}

np::scene::LoadingScene::LoadingCallbackResult
LoadingCallback()
{
	np::api::TriggerInboxDownload();

	return np::scene::LoadingScene::LOADING_CALLBACK_RESULT_NEXT_SCENE;
}

}  // namespace

namespace np { namespace scene {

	LocationScene::LocationScene(void)
	{
		NN_LOG_INFO("Initializing np::scene::LocationScene");

		m_NumOptions = 4;
		m_Options	 = new Option*[m_NumOptions];

		m_Options[0] = new Option();
		m_Options[1] = new Option();
		m_Options[2] = new Option();
		m_Options[3] = new Option();

		m_Options[0]->text = "Trigger box download";
		m_Options[1]->text = "Request passes";
		m_Options[2]->text = "Settings";
		m_Options[3]->text = "Exit";

		m_Options[0]->callback = reinterpret_cast<OptionCallback>(&LocationScene::OnTriggerBoxDownloadSelected);
		m_Options[1]->callback = reinterpret_cast<OptionCallback>(&LocationScene::OnRequestPassesSelected);
		m_Options[2]->callback = reinterpret_cast<OptionCallback>(&LocationScene::OnSettingsSelected);
		m_Options[3]->callback = reinterpret_cast<OptionCallback>(&LocationScene::OnExitSelected);
	}

	LocationScene::~LocationScene(void)
	{
		NN_LOG_INFO("Finalizing np::scene::LocationScene");

		for (u32 i = 0; i < m_NumOptions; i++)
			delete m_Options[i];

		delete[] m_Options;
	}

	void LocationScene::DrawOptions(np::graphics::RenderSystemDrawing* renderSystem)
	{
		renderSystem->SetRenderTarget(NN_GX_DISPLAY0);
		renderSystem->Clear();

		// Title bar
		renderSystem->DrawText(0, 0, "You are at the %s", CurrentLocationToString());

		m_OptionsOffset = 2;

		OptionSceneBase::DrawOptions(renderSystem, false);
	}

	void LocationScene::OnSettingsSelected(void)
	{
		NN_LOG_INFO("Switching to settings scene");

		SettingsScene::SetLastScene(GetLocationScene());

		ChangeScene(GetSettingsScene());
	}

	void LocationScene::OnExitSelected(void)
	{
		NN_LOG_INFO("Exiting application");

		Exit();
	}

	void LocationScene::OnTriggerBoxDownloadSelected(void)
	{
		NN_LOG_INFO("Trigger box download selected");

		LoadingScene* loadingScene = GetLoadingScene();
		loadingScene->Update(GetLocationScene(), reinterpret_cast<LoadingScene::LoadingCallback>(&LoadingCallback));

		ChangeScene(loadingScene);
	}

	void LocationScene::OnRequestPassesSelected(void)
	{
		NN_LOG_INFO("Request passes selected");

		ChangeScene(GetRequestPassesScene());
	}

}}	// namespace np::scene
