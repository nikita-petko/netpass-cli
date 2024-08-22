#ifndef NP_SCENE_SCENES_SCENE_TOGGLETITLESCENE_H_
#define NP_SCENE_SCENES_SCENE_TOGGLETITLESCENE_H_

#include <np/scene/scene_SceneBase.h>

#include <nn/cec.h>

namespace np { namespace scene {
	/*
		@brief Scene that is rendered if the user selects the toggle titles option.
	*/
	class ToggleTitleScene : public OptionSceneBase
	{
		/*!
			@brief Callback for when the user selects a title.
		*/
		static void OnToggleTitleSelected();

		/*!
			@brief Callback for when the user selects the return to the last scene option.
		*/
		static void OnReturnToLastSceneSelected();

	public:
		/*!
			@brief Constructor.
		*/
		ToggleTitleScene(void);

		/*!
			@brief Destructor.
		*/
		virtual ~ToggleTitleScene(void);

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
		nn::cec::TitleId* m_IndexToTitleIdMap;
	};

}}	// namespace np::scene

#endif	// NP_SCENE_SCENES_SCENE_TOGGLETITLESCENE_H_
