#ifndef NP_SCENE_SCENES_SCENE_SWITCHSCENE_H_
#define NP_SCENE_SCENES_SCENE_SWITCHSCENE_H_

#include <np/scene/scene_SceneBase.h>

namespace np { namespace scene {

	/*
		@brief A scene that will swap to another scene depending on the result of a callback function.
	*/
	class SwitchScene : public SceneBase
	{
	public:
		/*!
			@brief Callback for switching scenes.

			@return The scene to switch to.
		*/
		typedef SceneBase* (*SwitchCallback)();

		/*!
			@brief Updates the scene to transition to after the callback has been called.

			@param[in] switchCallback The callback to call when switching scenes.
		*/
		void Update(SwitchCallback switchCallback);

		/*!
			@overload
		*/
		virtual void OnSceneChange(void);
	private:
		SwitchCallback m_SwitchCallback;
	};

}}	// namespace np::scene

#endif	// NP_SCENE_SCENES_SCENE_SWITCHSCENE_H_
