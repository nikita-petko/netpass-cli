#include <np/scene/scene_SceneBase.h>
#include <np/scene/scene_SceneManager.h>

#include <np/graphics.h>

#include <np/log.h>

namespace np { namespace scene {

	void OptionSceneBase::Step(const nn::hid::PadStatus& padStatus)
	{
		if ((padStatus.trigger & nn::hid::BUTTON_UP) || (padStatus.trigger & nn::hid::BUTTON_EMULATION_UP))
		{
			// Handle jumping to the bottom of the list when trying to up from the very top (m_SelectedOption == 0, set m_SelectedOption to
			// m_NumOptions - 1)
			if (m_SelectedOption == 0)
				m_SelectedOption = m_NumOptions - 1;
			else
				do { --m_SelectedOption; } while (m_Options[m_SelectedOption]->blank);

			Render();
		}
		else if ((padStatus.trigger & nn::hid::BUTTON_DOWN) || (padStatus.trigger & nn::hid::BUTTON_EMULATION_DOWN))
		{
			// Handle jumping to the top of the list when trying to down from the very bottom (m_SelectedOption == m_NumOptions - 1, set
			// m_SelectedOption to 0)
			if (m_SelectedOption == m_NumOptions - 1)
				m_SelectedOption = 0;
			else
				do { ++m_SelectedOption; } while (m_Options[m_SelectedOption]->blank);

			Render();
		}
		else if (padStatus.trigger & nn::hid::BUTTON_A)
		{
			m_Options[m_SelectedOption]->callback();
		}
	}

	void OptionSceneBase::DrawOptions(np::graphics::RenderSystemDrawing* renderSystem, bool clear)
	{
		renderSystem->SetRenderTarget(NN_GX_DISPLAY0);

		if (clear)
			renderSystem->Clear();

		// Draw options
		// selected option has black text and white background
		// other options have white text and black background

		u8 offset = 0;
		if (m_OptionsOffset > 0)
		{
			offset = m_OptionsOffset + 1;
		}

		for (u8 i = 0; i < m_NumOptions; ++i)
		{
			if (m_Options[i]->blank)
				continue;

			if (i == m_SelectedOption)
			{
				NN_LOG_DEBUG("Selected option: %d, offset: %d, offset pixels: %d", i, offset, LINE(i + offset));

				// Draw selected option
				if (m_Options[i]->color)
					renderSystem->SetColor(m_Options[i]->color[0], m_Options[i]->color[1], m_Options[i]->color[2], m_Options[i]->color[3]);
				else
					renderSystem->SetColor(GRAPHICS_COLOR_BLACK);

				renderSystem->DrawText(0, LINE(i + offset), m_Options[i]->text);

				renderSystem->SetColor(GRAPHICS_COLOR_WHITE);
				renderSystem->FillRectangle(0, LINE(i + offset), CHARS(std::strlen(m_Options[i]->text) + 1), FONT_SIZE);
			}
			else
			{
				NN_LOG_DEBUG("Unselected option: %d, offset: %d, offset pixels: %d", i, offset, LINE(i + offset));

				// Draw unselected option
				if (m_Options[i]->color)
					renderSystem->SetColor(m_Options[i]->color[0], m_Options[i]->color[1], m_Options[i]->color[2], m_Options[i]->color[3]);
				else
					renderSystem->SetColor(GRAPHICS_COLOR_WHITE);

				renderSystem->DrawText(0, LINE(i + offset), m_Options[i]->text);
			}
		}

		renderSystem->SwapBuffers();
	}

}}	// namespace np::scene
