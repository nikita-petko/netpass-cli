#ifndef NP_SCENE_SCENES_SCENE_LOADINGSCENE_H_
#define NP_SCENE_SCENES_SCENE_LOADINGSCENE_H_

#include <np/scene/scene_SceneBase.h>

namespace np { namespace scene {
	/*!
		@brief  A scene that is displayed while loading resources.
	*/
	class LoadingScene : public SceneBase
	{
	public:
		/*!
			@brief Result of the loading callback.
		*/
		enum LoadingCallbackResult
		{
			LOADING_CALLBACK_RESULT_NEXT_SCENE,			 // Proceed to the next scene
			LOADING_CALLBACK_RESULT_RETRY,				 // Retry loading
			LOADING_CALLBACK_RESULT_ERROR,				 // An error occurred, set error code within the callback
			LOADING_CALLBACK_RESULT_NONE,				 // It isn't an error, but do not continue to the next scene
			LOADING_CALLBACK_RESULT_NEXT_SCENE_ON_CLICK	 // Proceed to the next scene when the user clicks the button specified in
														 // Initialize()
		};

		/*!
			@brief Callback for loading resources.

			@return The result of the loading callback.
		*/
		typedef LoadingCallbackResult (*LoadingCallback)();

		/*!
			@brief Updates the scene to transition to after the loading callback has been called.

			@param[in] nextScene The scene to transition to.
			@param[in] loadingCallback The callback to call when loading resources.
			@param[in] nextSceneButton The button to press to transition to the next scene, if not specified, the A button is used.
		*/
		void Update(SceneBase* nextScene, LoadingCallback loadingCallback, bit32 nextSceneButton = nn::hid::BUTTON_A);

		/*!
			@brief Overrides the next scene to transition to after the loading callback has been called.
				   Handy for when you only want to change the next scene and not the loading callback.
			
			@param[in] nextScene The scene to transition to.
		*/
		void SetNextScene(SceneBase* nextScene) { m_NextScene = nextScene; }

		/*!
			@overload
		*/
		virtual void OnSceneChange(void);

		/*!
			@overload
		*/
		virtual void Step(const nn::hid::PadStatus& padStatus);

	private:
		/*!
			@brief Callback for the graphics command to render the loading screen.

			@param[in] renderSystem The render system to use.
			@param[in] param The parameter to pass to the render function.
		*/
		static void RenderLoadingScreen(np::graphics::RenderSystemDrawing* renderSystem, uptr param);

		SceneBase*		m_NextScene;
		LoadingCallback m_LoadingCallback;
		bit32			m_NextSceneButton;
		bool			m_NextSceneOnButtonClickEnabled;
	};

}}	// namespace np::scene

#endif	// NP_SCENE_SCENES_SCENE_LOADINGSCENE_H_
