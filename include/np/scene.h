#ifndef NP_SCENE_H_
#define NP_SCENE_H_

#if defined(NN_PLATFORM_CTR)
#	ifdef __cplusplus
#		include <np/scene/scenes/scene_HelpScene.h>
#		include <np/scene/scenes/scene_HomeScene.h>
#		include <np/scene/scenes/scene_LoadingScene.h>
#		include <np/scene/scenes/scene_LocationScene.h>
#		include <np/scene/scenes/scene_RequestPassesScene.h>
#		include <np/scene/scenes/scene_SettingsScene.h>
#		include <np/scene/scenes/scene_SwitchScene.h>
#		include <np/scene/scenes/scene_ToggleTitleScene.h>

#		include <np/scene/scene_SceneBase.h>
#		include <np/scene/scene_SceneManager.h>
#	endif
#else
#	error no platform selected
#endif	// NN_PLATFORM_CTR

#endif	// NP_SCENE_H_
