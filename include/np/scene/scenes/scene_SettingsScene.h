#ifndef NP_SCENE_SCENES_SCENE_SETTINGSSCENE_H_
#define NP_SCENE_SCENES_SCENE_SETTINGSSCENE_H_

#include <np/scene/scene_SceneBase.h>

namespace np { namespace scene {
	/*
		@brief Scene that is rendered if the user selects the settings option.
			   Displays the options to toggle titles and show help.
	*/
	class SettingsScene : public OptionSceneBase
	{
		/*!
			@brief Callback for when the user selects the toggle titles option.
		*/
		static void OnToggleTitlesSelected();

		/*!
			@brief Callback for when the user selects the toggle show help option.
		*/
		static void OnToggleShowHelpSelected();

		/*!
			@brief Callback for when the user selects the return to the last scene option.
		*/
		static void OnReturnToLastSceneSelected();

	public:
		/*!
			@brief Constructor.
		*/
		SettingsScene(void);

		/*!
			@brief Destructor.
		*/
		virtual ~SettingsScene(void);

		/*!
			@brief Sets the last scene to return to when the user selects the return to the last scene option.

			@param[in] lastScene The last scene to return to.
		*/
		static void SetLastScene(SceneBase* lastScene);

		/*!
			@overload
		*/
		virtual void Step(const nn::hid::PadStatus& padStatus);

	protected:
		/*!
			@overload
		*/
		virtual void DrawOptions(np::graphics::RenderSystemDrawing* renderSystem);

	private:
		SceneBase* m_LastScene;
	};

}}	// namespace np::scene

#endif	// NP_SCENE_SCENES_SCENE_SETTINGSSCENE_H_
