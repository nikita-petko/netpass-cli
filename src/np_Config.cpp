#include <np/api.h>
#include <np/config.h>
#include <np/log.h>

#include <nn/cec.h>
#include <nn/fs.h>

#include <np/log_Macros.h>

namespace {
np::config::Configuration* s_Configuration;

nn::fs::FileStream s_ConfigFile;

void
ClearNonExistingTitles()
{
	NN_LOG_INFO("Clearing non-existing titles");

	for (u32 i = 0; i < 12; i++)
	{
		if (s_Configuration->disabled_title_ids[i] == 0)
			continue;

		nn::cec::MessageBoxInfo dummy;
		nn::Result				result = np::api::GetMessageBoxInfo(s_Configuration->disabled_title_ids[i], &dummy);
		if (result == nn::cec::ResultNoData())
		{
			NN_LOG_INFO("Clearing non-existing title ID: 0x%016llX", s_Configuration->disabled_title_ids[i]);

			s_Configuration->disabled_title_ids[i] = 0;
		}
	}
}
}  // namespace

namespace np { namespace config {

	void Initialize(void)
	{
		if (s_Configuration)
			return;

		NN_LOG_INFO("Initializing np::config");

		nn::Result result;

		result = nn::fs::MountSaveData();

		if (result.IsFailure())
			if (result <= nn::fs::ResultNotFormatted() || result <= nn::fs::ResultBadFormat() ||
				result <= nn::fs::ResultVerificationFailed())
			{
				NN_LOG_WARN("Formatting save data (result: 0x%08X)", result.GetPrintableBits());

				result = nn::fs::FormatSaveData(1, 1);

				result = nn::fs::MountSaveData();
				if (result.IsFailure())
				{
					NN_LOG_FATAL("Failed to mount save data (result: 0x%08X)", result.GetPrintableBits());
					return;
				}
			}

		NN_PANIC_IF_FAILED(
			s_ConfigFile.TryInitialize("data:/config.bin", nn::fs::OPEN_MODE_CREATE | nn::fs::OPEN_MODE_READ | nn::fs::OPEN_MODE_WRITE));

		s_Configuration = new Configuration;

		if (s_ConfigFile.GetSize() != sizeof(Configuration))
		{
			s_Configuration->show_help = true;	// default to showing help
			s_Configuration->last_location = -1;  // default to no location

			NN_LOG_INFO("Creating new configuration file, or overwriting existing one, s_Configuration = 0x%p", s_Configuration);

			s_ConfigFile.SetSize(sizeof(Configuration));

			FlushConfiguration();
		}
		else
		{
			NN_LOG_INFO("Reading configuration file, s_Configuration = 0x%p", s_Configuration);

			s32 bytesRead = 0;
			NN_PANIC_IF_FAILED(s_ConfigFile.TryRead(&bytesRead, s_Configuration, sizeof(Configuration)));
		}

		NN_PANIC_IF_FAILED(s_ConfigFile.TrySeek(0, nn::fs::POSITION_BASE_BEGIN));

		ClearNonExistingTitles();
	}

	void Finalize(void)
	{
		if (!s_Configuration)
			return;

		NN_LOG_INFO("Finalizing np::config");

		FlushConfiguration();

		delete s_Configuration;

		s_ConfigFile.Finalize();
	}

	Configuration& GetConfiguration(void)
	{
		return *s_Configuration;
	}

	void FlushConfiguration(void)
	{
		NN_LOG_INFO("Flushing configuration to file");

		s32 bytesWritten = 0;
		NN_PANIC_IF_FAILED(s_ConfigFile.TryWrite(&bytesWritten, s_Configuration, sizeof(Configuration), true));
		NN_PANIC_IF_FAILED(s_ConfigFile.TrySeek(0, nn::fs::POSITION_BASE_BEGIN));  // Ensure we are alway overwriting the file
	}

	bool IsTitleIdIgnored(nn::cec::TitleId titleId)
	{
		for (u32 i = 0; i < 12; i++)
		{
			if (s_Configuration->disabled_title_ids[i] == titleId)
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
		for (u32 i = 0; i < 12; i++)
		{
			if (s_Configuration->disabled_title_ids[i] == 0)
			{
				s_Configuration->disabled_title_ids[i] = titleId;
				return;
			}
		}

		NN_LOG_WARN("No more room to add ignored title ID: 0x%016llX", titleId);
	}

	void RemoveIgnoredTitle(nn::cec::TitleId titleId)
	{
		for (u32 i = 0; i < 12; i++)
		{
			if (s_Configuration->disabled_title_ids[i] == titleId)
			{
				s_Configuration->disabled_title_ids[i] = 0;
				return;
			}
		}

		NN_LOG_WARN("Title ID: 0x%016llX not found in ignored list", titleId);
	}
}}	// namespace np::config
