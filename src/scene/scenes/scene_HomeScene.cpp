#include <np/scene/scene_SceneManager.h>
#include <np/scene/scenes/scene_HomeScene.h>

#include <nn/http.h>

#include <np/api.h>
#include <np/config.h>
#include <np/graphics.h>

#include <np/log.h>

namespace {

using namespace np::scene;

LoadingScene::LoadingCallbackResult
LoadingCallback(void)
{
	nn::Result result = np::api::SetLocation(np::api::GetLocalLocation());
	if (!result.IsSuccess())
	{
		if (result == nn::http::ResultTimeout())
		{
			NN_LOG_ERROR("HTTP request timed out, retrying");

			return LoadingScene::LOADING_CALLBACK_RESULT_RETRY;
		}

		NN_LOG_ERROR("Failed to set location:");
		NN_DBG_PRINT_RESULT(result);

		np::graphics::DrawFatality("ERROR SETTING LOCATION");
		PRINTF_DISPLAY1("Failed to set location");

		return LoadingScene::LOADING_CALLBACK_RESULT_ERROR;
	}

	np::api::TriggerInboxDownload();

	return LoadingScene::LOADING_CALLBACK_RESULT_NEXT_SCENE;
}

}  // namespace

namespace np { namespace scene {

	void HomeScene::DrawOptions(np::graphics::RenderSystemDrawing* renderSystem)
	{
		renderSystem->SetRenderTarget(NN_GX_DISPLAY0);
		renderSystem->Clear();

		renderSystem->SetColor(GRAPHICS_COLOR_WHITE);

		// Title bar
		renderSystem->DrawText(0, 0, "You are at home");

		m_OptionsOffset = 2;

		OptionSceneBase::DrawOptions(renderSystem, false);
	}

	HomeScene::HomeScene(void)
	{
		NN_LOG_INFO("Initializing np::scene::HomeScene");

		m_NumOptions = 8;

#ifdef NN_BUILD_DEBUG
		m_NumOptions += 1;	// Add an extra option for debugging
#endif

		m_Options = new Option*[m_NumOptions];

		m_Options[0] = new Option();
		m_Options[1] = new Option();
		m_Options[2] = new Option();
		m_Options[3] = new Option();
		m_Options[4] = new Option();
		m_Options[5] = new Option();
		m_Options[6] = new Option();
		m_Options[7] = new Option();

		// 6 options are for the 6 locations
		// 1 is for switching to the settings scene
		// 1 is for exiting the application
		m_Options[0]->text = "Go to Train Station";
		m_Options[1]->text = "Go to Plaza";
		m_Options[2]->text = "Go to Mall";
		m_Options[3]->text = "Go to the Beach";
		m_Options[4]->text = "Go to the Arcade";
		m_Options[5]->text = "Go to the Cat Cafe";
		m_Options[6]->text = "Settings";
		m_Options[7]->text = "Exit";

		m_Options[0]->callback = reinterpret_cast<OptionCallback>(&HomeScene::OnLocationSelected);
		m_Options[1]->callback = reinterpret_cast<OptionCallback>(&HomeScene::OnLocationSelected);
		m_Options[2]->callback = reinterpret_cast<OptionCallback>(&HomeScene::OnLocationSelected);
		m_Options[3]->callback = reinterpret_cast<OptionCallback>(&HomeScene::OnLocationSelected);
		m_Options[4]->callback = reinterpret_cast<OptionCallback>(&HomeScene::OnLocationSelected);
		m_Options[5]->callback = reinterpret_cast<OptionCallback>(&HomeScene::OnLocationSelected);
		m_Options[6]->callback = reinterpret_cast<OptionCallback>(&HomeScene::OnSettingsSelected);
		m_Options[7]->callback = reinterpret_cast<OptionCallback>(&HomeScene::OnExitSelected);

#ifdef NN_BUILD_DEBUG
		m_Options[8]		   = new Option();
		m_Options[8]->text	   = "Force go to location scene";
		m_Options[8]->callback = reinterpret_cast<OptionCallback>(&HomeScene::ForceGoToLocationScene);
		m_Options[8]->color	   = np::graphics::COLOR_RED;
#endif
	}

	HomeScene::~HomeScene(void)
	{
		NN_LOG_INFO("Finalizing np::scene::HomeScene");

		for (u32 i = 0; i < m_NumOptions; i++)
			delete m_Options[i];

		delete[] m_Options;
	}

#ifdef NN_BUILD_DEBUG
	void HomeScene::ForceGoToLocationScene(void)
	{
		NN_LOG_INFO("Forcing go to location scene");

		ChangeScene(GetLocationScene());
	}
#endif

	void HomeScene::OnLocationSelected(void)
	{
		HomeScene* homeScene = GetHomeScene();

		NN_LOG_INFO("Location selected %d", homeScene->m_SelectedOption);

		if (np::config::GetConfiguration()->last_location == homeScene->m_SelectedOption)
		{
			NN_LOG_INFO("Previously at location %d", homeScene->m_SelectedOption);

			PRINTF_DISPLAY1("You can't go to the same location twice in a row!");

			ChangeScene(GetHomeScene());  // Reset the scene

			return;
		}

		np::api::SetLocalLocation(homeScene->m_SelectedOption);

		LoadingScene* loadingScene = GetLoadingScene();
		loadingScene->Update(GetLocationScene(), LoadingCallback);

		ChangeScene(loadingScene);
	}

	void HomeScene::OnSettingsSelected(void)
	{
		NN_LOG_INFO("Settings selected");

		SettingsScene::SetLastScene(GetHomeScene());

		ChangeScene(GetSettingsScene());
	}

	void HomeScene::OnExitSelected(void)
	{
		NN_LOG_INFO("Exit selected");

		Exit();
	}

}}	// namespace np::scene
