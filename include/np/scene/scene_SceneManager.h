#ifndef NP_SCENE_SCENE_MANAGER_H_
#define NP_SCENE_SCENE_MANAGER_H_

#include <nn/hid.h>

#include <np/scene/scenes/scene_HelpScene.h>
#include <np/scene/scenes/scene_HomeScene.h>
#include <np/scene/scenes/scene_LoadingScene.h>
#include <np/scene/scenes/scene_LocationScene.h>
#include <np/scene/scenes/scene_RequestPassesScene.h>
#include <np/scene/scenes/scene_SettingsScene.h>
#include <np/scene/scenes/scene_SwitchScene.h>
#include <np/scene/scenes/scene_ToggleTitleScene.h>

namespace np { namespace scene {
	class SceneBase;

	/*!
		@brief Initializes np::scene.
	*/
	void Initialize(void);

	/*!
		@brief Finalizes np::scene.
	*/
	void Finalize(void);

	/*!
		@brief Changes the current scene to the specified scene.
	*/
	void ChangeScene(SceneBase* scene);

	/*!
		@brief Function called every frame to update the current scene.

		@return True if the application should continue running, false otherwise.
	*/
	bool Step(const nn::hid::PadStatus& padStatus);

	/*!
		@brief Prevents the current scene from being updated and exits the application.
	*/
	void Exit(void);

	/*!
		@brief Gets a pointer to np::scene::HelpScene.

		@return A pointer to np::scene::HelpScene.
	*/
	HelpScene* GetHelpScene(void);

	/*!
		@brief Gets a pointer to np::scene::LoadingScene.

		@return A pointer to np::scene::LoadingScene.
	*/
	LoadingScene* GetLoadingScene(void);

	/*!
		@brief Gets a pointer to np::scene::SwitchScene.

		@return A pointer to np::scene::SwitchScene.
	*/
	SwitchScene* GetSwitchScene(void);

	/*!
		@brief Gets a pointer to np::scene::HomeScene.

		@return A pointer to np::scene::HomeScene.
	*/
	HomeScene* GetHomeScene(void);

	/*!
		@brief Gets a pointer to np::scene::LocationScene.

		@return A pointer to np::scene::LocationScene.
	*/
	LocationScene* GetLocationScene(void);

	/*!
		@brief Gets a pointer to np::scene::SettingsScene.

		@return A pointer to np::scene::SettingsScene.
	*/
	SettingsScene* GetSettingsScene(void);

	/*!
		@brief Gets a pointer to np::scene::ToggleTitleScene.

		@return A pointer to np::scene::ToggleTitleScene.
	*/
	ToggleTitleScene* GetToggleTitleScene(void);

	/*!
		@brief Gets a pointer to np::scene::RequestPassesScene.

		@return A pointer to np::scene::RequestPassesScene.
	*/
	RequestPassesScene* GetRequestPassesScene(void);

	/*!
		@brief Checks if the specified scene is active.

		@param[in] scene The scene to check.

		@return True if the scene is active, false otherwise.
	*/
	bool IsActive(SceneBase* scene);

}}	// namespace np::scene

#endif	// NP_SCENE_SCENE_MANAGER_H_
