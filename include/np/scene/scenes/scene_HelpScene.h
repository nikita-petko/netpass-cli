#ifndef NP_SCENE_SCENES_SCENE_HELPSCENE_H_
#define NP_SCENE_SCENES_SCENE_HELPSCENE_H_

#include <np/scene/scene_SceneBase.h>

namespace np { namespace scene {
	/*!
		@brief Scene that is rendered if np::config::Configuration::show_help is true.
			   Displays some basic information about the application, such as the description
			   and the controls.
	*/
	class HelpScene : public SceneBase
	{
	private:
		/*!
			@brief Callback for the graphics command to render the help screen.

			@param[in] renderSystem The render system to use.
			@param[in] param The parameter to pass to the render function.
		*/
		static void RenderHelpScreen(np::graphics::RenderSystemDrawing* renderSystem, uptr param);

	public:
		/*!
			@brief Goes to the next scene in the scene stack.
			@note This is here in the case that the user has configured the scene to be skipped.
		*/
		static void NextScene(void);  // Public, in case the user has configured the scene to be skipped

		/*!
			@overload
		*/
		virtual void OnSceneChange(void);

		/*!
			@overload
		*/
		virtual void Step(const nn::hid::PadStatus& padStatus);
	};
}}	// namespace np::scene

#endif	// NP_SCENE_SCENES_SCENE_HELPSCENE_H_
