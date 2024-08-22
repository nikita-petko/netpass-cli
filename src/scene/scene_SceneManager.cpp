#include <np/scene/scene_SceneBase.h>
#include <np/scene/scene_SceneManager.h>

#include <np/graphics.h>

#include <np/log.h>

namespace {
np::scene::SceneBase* s_CurrentScene;

bool s_Running = true;

np::scene::HelpScene*		   s_HelpScene;
np::scene::LoadingScene*	   s_LoadingScene;
np::scene::SwitchScene*		   s_SwitchScene;
np::scene::HomeScene*		   s_HomeScene;
np::scene::LocationScene*	   s_LocationScene;
np::scene::SettingsScene*	   s_SettingsScene;
np::scene::ToggleTitleScene*   s_ToggleTitleScene;
np::scene::RequestPassesScene* s_RequestPassesScene;
}  // namespace

namespace np { namespace scene {

	void Initialize(void)
	{
		NN_LOG_INFO("Initializing np::scene");

		s_HelpScene			 = new HelpScene();
		s_LoadingScene		 = new LoadingScene();
		s_SwitchScene		 = new SwitchScene();
		s_HomeScene			 = new HomeScene();
		s_LocationScene		 = new LocationScene();
		s_SettingsScene		 = new SettingsScene();
		s_ToggleTitleScene	 = new ToggleTitleScene();
		s_RequestPassesScene = new RequestPassesScene();
	}

	void Finalize(void)
	{
		NN_LOG_INFO("Finalizing np::scene");

		delete s_HelpScene;
		delete s_LoadingScene;
		delete s_SwitchScene;
		delete s_HomeScene;
		delete s_LocationScene;
		delete s_SettingsScene;
		delete s_ToggleTitleScene;
		delete s_RequestPassesScene;
	}

	HelpScene* GetHelpScene(void)
	{
		return s_HelpScene;
	}

	LoadingScene* GetLoadingScene(void)
	{
		return s_LoadingScene;
	}

	SwitchScene* GetSwitchScene(void)
	{
		return s_SwitchScene;
	}

	HomeScene* GetHomeScene(void)
	{
		return s_HomeScene;
	}

	LocationScene* GetLocationScene(void)
	{
		return s_LocationScene;
	}

	SettingsScene* GetSettingsScene(void)
	{
		return s_SettingsScene;
	}

	ToggleTitleScene* GetToggleTitleScene(void)
	{
		return s_ToggleTitleScene;
	}

	RequestPassesScene* GetRequestPassesScene(void)
	{
		return s_RequestPassesScene;
	}

	void Exit(void)
	{
		s_Running = false;
	}

	void ChangeScene(SceneBase* scene)
	{
		NN_LOG_INFO("Changing scene to %p", scene);

		s_CurrentScene = scene;
		s_CurrentScene->OnSceneChange();
	}

	bool Step(const nn::hid::PadStatus& padStatus)
	{
		if (!s_Running)
			return false;

		s_CurrentScene->Step(padStatus);

		return true;
	}

	bool IsActive(SceneBase* scene)
	{
		return s_CurrentScene == scene;
	}

}}	// namespace np::scene
