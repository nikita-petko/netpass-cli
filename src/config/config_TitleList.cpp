#include <np/api.h>
#include <np/config.h>

#include <nn/cec.h>
#include <nn/fs.h>

#include <np/log.h>

namespace np { namespace config {

	bool IsTitleIdIgnored(nn::cec::TitleId titleId)
	{
        Configuration* configuration = GetConfiguration();

		for (u32 i = 0; i < 12; i++)
		{
			if (configuration->disabled_title_ids[i] == titleId)
				return true;
		}

		return false;
	}

	void ClearIgnoredTitles(nn::cec::MessageBoxList* pIn)
	{
		NN_LOG_INFO("Clearing ignored titles");

		size_t position = 0;

		for (u32 i = 0; i < pIn->DirNum; i++)
		{
			nn::cec::TitleId titleId = std::strtoul(reinterpret_cast<const char*>(pIn->DirName[i]), NULL, 16);
			if (!IsTitleIdIgnored(titleId))
			{
				if (position != i)
					std::memcpy(pIn->DirName[position], pIn->DirName[i], sizeof(pIn->DirName[i]));

				position++;
			}
		}

		std::memset(pIn->DirName[position], 0, sizeof(pIn->DirName[position]) * (pIn->DirNum - position));
		pIn->DirNum = position;
	}

	void AddIgnoredTitle(nn::cec::TitleId titleId)
	{
        Configuration* configuration = GetConfiguration();

		for (u32 i = 0; i < 12; i++)
		{
			if (configuration->disabled_title_ids[i] == 0)
			{
				configuration->disabled_title_ids[i] = titleId;
				return;
			}
		}

		NN_LOG_WARN("No more room to add ignored title ID: 0x%016llX", titleId);
	}

	void RemoveIgnoredTitle(nn::cec::TitleId titleId)
	{
        Configuration* configuration = GetConfiguration();

		for (u32 i = 0; i < 12; i++)
		{
			if (configuration->disabled_title_ids[i] == titleId)
			{
				configuration->disabled_title_ids[i] = 0;
				return;
			}
		}

		NN_LOG_WARN("Title ID: 0x%016llX not found in ignored list", titleId);
	}
}}	// namespace np::config
