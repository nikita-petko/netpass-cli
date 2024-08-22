#ifndef NP_SCENE_SCENES_SCENE_HOMESCENE_H_
#define NP_SCENE_SCENES_SCENE_HOMESCENE_H_

#include <np/scene/scene_SceneBase.h>

namespace np { namespace scene {

	/*!
		@brief The first scene to render if you are not in a location.
	*/
	class HomeScene : public OptionSceneBase
	{
		/*!
			@brief Callback for when the user selects a location.
		*/
		static void OnLocationSelected();

		/*!
			@brief Callback for when the user selects the settings option.
		*/
		static void OnSettingsSelected();

		/*!
			@brief Callback for when the user selects the exit option.
		*/
		static void OnExitSelected();

#ifdef NN_BUILD_DEBUG
		// Goes to the location without checking or setting
		// the location on the server, only for debugging specific
		// actions on the location scene while staying outside
		// of an actual location.
		static void ForceGoToLocationScene();
#endif

	public:
		/*!
			@brief Constructor.
		*/
		HomeScene(void);

		/*!
			@brief Destructor.
		*/
		virtual ~HomeScene(void);

	protected:
		/*!
			@overload
		*/
		virtual void DrawOptions(np::graphics::RenderSystemDrawing* renderSystem);
	};

}}	// namespace np::scene

#endif	// NP_SCENE_SCENES_SCENE_HOMESCENE_H_
