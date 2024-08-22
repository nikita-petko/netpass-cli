#include <np/scene/scene_SceneManager.h>
#include <np/scene/scenes/scene_SwitchScene.h>

#include <np/graphics.h>

#include <np/log_Macros.h>

namespace np { namespace scene {

	void SwitchScene::OnSceneChange(void)
	{
		NN_LOG_INFO("SwitchScene::OnSceneChange");

		ChangeScene(m_SwitchCallback());
	}

	void SwitchScene::Update(SwitchCallback switchCallback)
	{
		m_SwitchCallback = switchCallback;
	}

}}	// namespace np::scene
