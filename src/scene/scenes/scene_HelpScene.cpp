#include <np/scene/scene_SceneManager.h>
#include <np/scene/scenes/scene_HelpScene.h>

#include <np/api.h>
#include <np/config.h>
#include <np/graphics.h>
#include <np/http.h>
#include <np/memory.h>

#include <np/log_Macros.h>

namespace {

using namespace np::scene;

SceneBase*
OnSwitch(void)
{
	s8 currentLocation = np::api::s_Location;

	nn::Result result = np::api::StartBackgroundThread();
	if (!result.IsSuccess())
	{
		NN_LOG_ERROR("Failed to start background thread:");
		NN_DBG_PRINT_RESULT(result);

		np::graphics::DrawFatality("ERROR STARTING THREAD");
		PRINTF_DISPLAY1("Failed to start thread");

		return NULL;
	}

	if (currentLocation == -1)
	{
		// Location is home
		// Switch to HomeScene
		return np::scene::GetHomeScene();
	}
	else
	{
		// Location is not home
		// Switch to StreetPassScene
		return np::scene::GetLocationScene();
	}
}

LoadingScene::LoadingCallbackResult LoadingCallback(void);

void
ResetRenderView(np::graphics::RenderSystemDrawing* renderSystem, uptr param)
{
	NN_UNUSED_VAR(param);

	renderSystem->SetRenderTarget(NN_GX_DISPLAY0);
	renderSystem->SetClearColor(NN_GX_DISPLAY0, GRAPHICS_COLOR_BLACK);
	renderSystem->Clear();

	renderSystem->SwapBuffers();

	renderSystem->SetRenderTarget(NN_GX_DISPLAY1);
	renderSystem->SetClearColor(NN_GX_DISPLAY1, GRAPHICS_COLOR_BLACK);
	renderSystem->Clear();

	renderSystem->SwapBuffers();
}

SceneBase*
OnSwitchBack(void)
{
	np::scene::LoadingScene* loadingScene = np::scene::GetLoadingScene();
	np::scene::SwitchScene*	 switchScene  = np::scene::GetSwitchScene();
	switchScene->Update(reinterpret_cast<SwitchScene::SwitchCallback>(&::OnSwitch));
	loadingScene->Update(switchScene, reinterpret_cast<LoadingScene::LoadingCallback>(&::LoadingCallback));

	return loadingScene;
}

SceneBase*
SwitchCleanUp(void)
{
	SUBMIT_RENDER_COMMAND(ResetRenderView);

	np::scene::SettingsScene* settingsScene = np::scene::GetSettingsScene();
	SwitchScene*			  switchScene	= GetSwitchScene();
	switchScene->Update(reinterpret_cast<SwitchScene::SwitchCallback>(&::OnSwitchBack));

	settingsScene->SetLastScene(switchScene);

	return settingsScene;
}

LoadingScene::LoadingCallbackResult
LoadingCallback(void)
{
	nn::Result result;

	result = np::api::UploadOutboxes();
	if (!result.IsSuccess())
	{
		if (result == np::api::ResultNoMessageBoxes())
		{
			NN_LOG_WARN("No message boxes to upload, we can't really receive any either");

			np::graphics::DrawStatusScreen(np::graphics::COLOR_ORANGE, "No outboxes to upload");

			CLEAR_DISPLAY1();
			PRINTF_DISPLAY1("You haven't enabled StreetPass on any");
			PRINTF_DISPLAY1("titles.");
			PRINTF_DISPLAY1("We can't receive any tags without any");
			PRINTF_DISPLAY1("enabled titles.");
			PRINTF_DISPLAY1("Go and turn on StreetPass on some titles");
			PRINTF_DISPLAY1("and then load this application again.");
			PRINTF_DISPLAY1(NULL);
			PRINTF_DISPLAY1(NULL);
			PRINTF_DISPLAY1("Press (start) to exit");
			PRINTF_DISPLAY1("or exit via the home menu.");

			return LoadingScene::LOADING_CALLBACK_RESULT_NONE;
		}

		if (result == np::api::ResultAllMessageBoxesDisabled())
		{
			NN_LOG_WARN("All message boxes are disabled, we can't really receive any either");

			np::graphics::DrawStatusScreen(np::graphics::COLOR_ORANGE, "All outboxes are disabled");

			CLEAR_DISPLAY1();
			PRINTF_DISPLAY1("All titles are disabled.");
			PRINTF_DISPLAY1("You will be sent to the settings to");
			PRINTF_DISPLAY1("enable titles.");
			PRINTF_DISPLAY1("After enabling titles, you can click on");
			PRINTF_DISPLAY1("the 'Return' option or press (B) to");
			PRINTF_DISPLAY1("return here.");
			PRINTF_DISPLAY1(NULL);
			PRINTF_DISPLAY1(NULL);
			PRINTF_DISPLAY1("Press (A) to continue.");
			PRINTF_DISPLAY1("Press (start) to exit");
			PRINTF_DISPLAY1("or exit via the home menu.");

			SwitchScene* switchScene = GetSwitchScene();
			switchScene->Update(reinterpret_cast<SwitchScene::SwitchCallback>(&::SwitchCleanUp));

			LoadingScene* loadingScene = GetLoadingScene();
			loadingScene->SetNextScene(switchScene);

			return LoadingScene::LOADING_CALLBACK_RESULT_NEXT_SCENE_ON_CLICK;
		}

		if (result == nn::http::ResultTimeout())
		{
			NN_LOG_ERROR("HTTP request timed out, retrying");

			return LoadingScene::LOADING_CALLBACK_RESULT_RETRY;
		}

		NN_LOG_ERROR("Failed to upload outboxes:");
		NN_DBG_PRINT_RESULT(result);

		np::graphics::DrawFatality("ERROR UPLOADING OUTBOXES");
		PRINTF_DISPLAY1("Failed to upload outboxes");

		return LoadingScene::LOADING_CALLBACK_RESULT_ERROR;
	}

	s8 location = -1;
	result		= np::api::GetLocation(&location);
	if (!result.IsSuccess())
	{
		if (result == nn::http::ResultTimeout())
		{
			NN_LOG_ERROR("HTTP request timed out, retrying");

			return LoadingScene::LOADING_CALLBACK_RESULT_RETRY;
		}

		NN_LOG_ERROR("Failed to get location:");
		NN_DBG_PRINT_RESULT(result);

		np::graphics::DrawFatality("ERROR GETTING LOCATION");
		PRINTF_DISPLAY1("Failed to get location");

		return LoadingScene::LOADING_CALLBACK_RESULT_ERROR;
	}

	if (location == -1)
		PRINTF_DISPLAY1("Location set to home");
	else
		PRINTF_DISPLAY1("Location set to %d", location);

	np::api::s_Location = location;

	return LoadingScene::LOADING_CALLBACK_RESULT_NEXT_SCENE;
}

}  // namespace

namespace np { namespace scene {

	void HelpScene::RenderHelpScreen(np::graphics::RenderSystemDrawing* renderSystem, uptr param)
	{
		NN_UNUSED_VAR(param);

		renderSystem->SetRenderTarget(NN_GX_DISPLAY0);

		// NetPass CLI (netpass is green, CLI is white)
		renderSystem->SetColor(GRAPHICS_COLOR_GREEN);
		renderSystem->DrawText(0, 0, "NetPass");
		renderSystem->SetColor(GRAPHICS_COLOR_WHITE);
		renderSystem->DrawText(CHARS(9), 0, "CLI");

		renderSystem->DrawText(0, LINE(2), "This is a more CLI like version of the NetPass");
		renderSystem->DrawText(0, LINE(3), "application.");
		renderSystem->DrawText(0, LINE(5), "What is NetPass?");
		renderSystem->DrawText(0, LINE(6), "NetPass is an application that allows you to");
		renderSystem->DrawText(0, LINE(7), "receive StreetPass (CEC) tags from other");
		renderSystem->DrawText(0, LINE(8), "3DS consoles via the internet.");
		renderSystem->DrawText(0, LINE(11), "Controls :");
		renderSystem->DrawText(0, LINE(12), "- (UP/DOWN) : Navigate");
		renderSystem->DrawText(0, LINE(13), "- (A) : Select or Confirm");
		renderSystem->DrawText(0, LINE(14), "- (B) : Back");
		renderSystem->DrawText(0, LINE(15), "- (START) : Exit");
		renderSystem->DrawText(0, LINE(17), "You can disable this screen in the settings.");

		renderSystem->SetColor(GRAPHICS_COLOR_GREEN);
		renderSystem->DrawText(0, LINE(19), "Press (A) to continue");
		renderSystem->SetColor(GRAPHICS_COLOR_WHITE);

		renderSystem->SwapBuffers();
	}

	void HelpScene::OnSceneChange(void)
	{
		NN_LOG_INFO("Transitioned to HelpScene");

		SUBMIT_RENDER_COMMAND(RenderHelpScreen);
	}

	void HelpScene::NextScene(void)
	{
		SwitchScene* switchScene = GetSwitchScene();
		switchScene->Update(reinterpret_cast<SwitchScene::SwitchCallback>(&::OnSwitch));

		LoadingScene* loadingScene = GetLoadingScene();
		loadingScene->Update(switchScene, reinterpret_cast<LoadingScene::LoadingCallback>(&::LoadingCallback));

		ChangeScene(loadingScene);
	}

	void HelpScene::Step(const nn::hid::PadStatus& padStatus)
	{
		if (padStatus.trigger & nn::hid::BUTTON_A)
			NextScene();
	}
}}	// namespace np::scene
