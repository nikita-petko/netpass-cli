#ifndef NP_SCENE_SCENES_SCENE_REQUESTPASSESSCENE_H_
#define NP_SCENE_SCENES_SCENE_REQUESTPASSESSCENE_H_

#include <np/scene/scene_SceneBase.h>

#include <nn/cec.h>

namespace np { namespace scene {
	/*
		@brief Scene that is rendered if the user selects the request passes option.
			   Displays the options to request passes for a specific title.
	*/
	class RequestPassesScene : public OptionSceneBase
	{
		/*!
			@brief Callback for when the user selects a title.
		*/
		static void OnTitleSelected();

		/*!
			@brief Callback for when the user selects the return to the last scene option.
		*/
		static void OnReturnToLastSceneSelected();

	public:
		/*!
			@brief Constructor.
		*/
		RequestPassesScene(void);

		/*!
			@brief Destructor.
		*/
		virtual ~RequestPassesScene(void);

		/*!
			@overload
		*/
		virtual void Step(const nn::hid::PadStatus& padStatus);

		/*!
			@brief Updates the title text for the specified title.
			@note This function is thread-safe.

			@param[in] titleId The title ID to update the text for.
			@param[in] boxMessages The number of messages in the box.
			@param[in] maxBoxMessages The maximum number of messages in the box.
		*/
		void UpdateTitleText(nn::cec::TitleId titleId, u32 boxMessages, u32 maxBoxMessages);

	protected:
		/*!
			@overload
		*/
		virtual void DrawOptions(np::graphics::RenderSystemDrawing* renderSystem);

	private:
		nn::cec::TitleId* m_IndexToTitleIdMap;
		char8**			  m_IndexToTitleNameMap;

		nn::os::CriticalSection m_CriticalSection;
	};
}}	// namespace np::scene

#endif	// NP_SCENE_SCENES_SCENE_REQUESTPASSESSCENE_H_
