#ifndef NP_SCENE_SCENE_BASE_H_
#define NP_SCENE_SCENE_BASE_H_

#include <nn/hid.h>

#include <np/graphics.h>

/*!
	@brief Macro to add a blank option to the options list.

	@param[in] index The index to add the blank option to.
*/
#define ADD_BLANK_OPTION(index)                 \
	{                                           \
		m_Options[index]		= new Option(); \
		m_Options[index]->blank = true;         \
	}

namespace np { namespace scene {
	/*!
		@brief Base class for all scenes.
	*/
	class SceneBase
	{
	public:
		/*!
			@brief Function called when the scene is changed to via np::scene::ChangeScene.
		*/
		virtual void OnSceneChange(void) {}

		/*!
			@brief Function called every frame to update the scene.

			@param[in] padStatus The current state of the controller.
		*/
		virtual void Step(const nn::hid::PadStatus& padStatus) { NN_UNUSED_VAR(padStatus); }
	};

	/*!
		@brief Base class for scenes that display a list of options.
	*/
	class OptionSceneBase : public SceneBase
	{
	public:
		/*!
			@brief Callback for when an option is selected.
		*/
		typedef void (*OptionCallback)();

		/*!
			@brief Structure for an option.
		*/
		struct Option
		{
			char*		   text;	  // The text to display for the option
			OptionCallback callback;  // The callback to call when the option is selected
			f32*		   color;	  // optional, if null it is just white (or black if it's the selected option)
			bool		   blank;  // optional, if true it will not draw the and will skip over it, useful for adding space between options
		};

		/*!
			@overload
		*/
		virtual void OnSceneChange(void)
		{
			m_SelectedOption = 0;
			m_OptionsOffset	 = 0;

			Render();
		}

		/*!
			@overload
		*/
		virtual void Step(const nn::hid::PadStatus& padStatus);

	protected:
		Option** m_Options;
		u8		 m_NumOptions;
		u8		 m_OptionsOffset;	// Offset for drawing options, use this if you want to draw something above the options
		u8		 m_SelectedOption;	// Highlighted option

		/*!
			@brief Function to draw the options, override this if you want to draw something above the options and call the base function.

			@param[in] renderSystem The render system to use.
		*/
		virtual void DrawOptions(np::graphics::RenderSystemDrawing* renderSystem) { DrawOptions(renderSystem, true); }

		/*!
			@brief Function to draw the options.

			@param[in] renderSystem The render system to use.
			@param[in] clear If true, the render system will clear the screen before drawing.
		*/
		void DrawOptions(np::graphics::RenderSystemDrawing* renderSystem, bool clear);

		/*!
			@brief Submits a render command to draw the options.
		*/
		void Render(void) { SUBMIT_RENDER_COMMAND_PARAM(DoRender, reinterpret_cast<uptr>(this)); }

	private:
		/*!
			@brief Callback for the graphics command to draw the options.

			@param[in] renderSystem The render system to use.
			@param[in] param The parameter to pass to the render function.
		*/
		static void DoRender(np::graphics::RenderSystemDrawing* renderSystem, uptr param)
		{
			OptionSceneBase* self = reinterpret_cast<OptionSceneBase*>(param);
			self->DrawOptions(renderSystem);
		}
	};
}}	// namespace np::scene

#endif	// NP_SCENE_SCENE_BASE_H_
