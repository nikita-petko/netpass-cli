#include <np/scene/scene_SceneManager.h>
#include <np/scene/scenes/scene_LoadingScene.h>

#include <np/graphics.h>

#include <np/log_Macros.h>

namespace np { namespace scene {

	void LoadingScene::RenderLoadingScreen(np::graphics::RenderSystemDrawing* renderSystem, uptr param)
	{
		NN_UNUSED_VAR(param);

		renderSystem->SetRenderTarget(NN_GX_DISPLAY0);
		renderSystem->Clear();
		renderSystem->SetColor(GRAPHICS_COLOR_WHITE);
		np::graphics::DrawCenteredText(renderSystem, "Loading...");
	}

	void LoadingScene::OnSceneChange(void)
	{
		NN_LOG_INFO("LoadingScene::OnSceneChange");

		SUBMIT_RENDER_COMMAND(RenderLoadingScreen);

		LoadingCallbackResult result = m_LoadingCallback();

		if (result == LOADING_CALLBACK_RESULT_NEXT_SCENE)
		{
			ChangeScene(m_NextScene);
		}
		else if (result == LOADING_CALLBACK_RESULT_RETRY)
		{
			// Retry loading
			ChangeScene(this);
		}
		else if (result == LOADING_CALLBACK_RESULT_ERROR)
		{
			// Error occurred
			NN_LOG_ERROR("Error occurred while loading");
		}
		else if (result == LOADING_CALLBACK_RESULT_NEXT_SCENE_ON_CLICK)
		{
			m_NextSceneOnButtonClickEnabled = true;
		}
	}

	void LoadingScene::Step(const nn::hid::PadStatus& padStatus)
	{
		if (m_NextSceneOnButtonClickEnabled && (padStatus.trigger & m_NextSceneButton))
			ChangeScene(m_NextScene);
	}

	void LoadingScene::Update(SceneBase* nextScene, LoadingCallback callback, bit32 nextSceneButton)
	{
		m_NextScene						= nextScene;
		m_LoadingCallback				= callback;
		m_NextSceneButton				= nextSceneButton;
		m_NextSceneOnButtonClickEnabled = false;
	}

}}	// namespace np::scene
