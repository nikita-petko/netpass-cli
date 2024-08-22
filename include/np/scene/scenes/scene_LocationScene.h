#ifndef NP_SCENE_SCENES_SCENE_LOCATIONSCENE_H_
#define NP_SCENE_SCENES_SCENE_LOCATIONSCENE_H_

#include <np/scene/scene_SceneBase.h>

namespace np { namespace scene {

	/*
		@brief Scene that is rendered if np::api::s_Location is not -1.
			   Displays the options to download the trigger box, request passes,
			   go to settings, or exit the application.
	*/
	class LocationScene : public OptionSceneBase
	{
		/*!
			@brief Callback for when the user selects the trigger box download option.
		*/
		static void OnTriggerBoxDownloadSelected();

		/*!
			@brief Callback for when the user selects the request passes option.
		*/
        static void OnRequestPassesSelected();

		/*!
			@brief Callback for when the user selects the settings option.
		*/
		static void OnSettingsSelected();

		/*!
			@brief Callback for when the user selects the exit option.
		*/
		static void OnExitSelected();

	public:

		/*!
			@brief Constructor.
		*/
		LocationScene(void);

		/*!
			@brief Destructor.
		*/
		virtual ~LocationScene(void);

	protected:
		virtual void DrawOptions(np::graphics::RenderSystemDrawing* renderSystem);
	};

}}	// namespace np::scene

#endif	// NP_SCENE_SCENES_SCENE_LOCATIONSCENE_H_
