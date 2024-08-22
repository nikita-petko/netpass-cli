#include <np/api.h>
#include <np/config.h>
#include <np/util.h>

#include <nn/cec.h>
#include <nn/fs.h>

#include <np/log.h>

#define CONFIG_FILE_PATH "data:/config.bin"

namespace {
np::config::Configuration* s_Configuration;
nn::fs::FileStream		   s_ConfigFile;

void
ClearNonExistingTitles()
{
	NN_LOG_INFO("Clearing non-existing titles");

	for (u32 i = 0; i < 12; i++)
	{
		if (s_Configuration->disabled_title_ids[i] == 0)
			continue;

		nn::cec::MessageBoxInfo dummy;
		nn::Result				result = np::util::GetCecMessageBoxInfo(s_Configuration->disabled_title_ids[i], &dummy);
		if (result == nn::cec::ResultNoData())
		{
			NN_LOG_INFO("Clearing non-existing title ID: 0x%016llX", s_Configuration->disabled_title_ids[i]);

			s_Configuration->disabled_title_ids[i] = 0;
		}
	}
}
}  // namespace

namespace np { namespace config {

	nn::Result Initialize(void)
	{
		if (s_Configuration)
			return nn::ResultSuccess();

		NN_LOG_INFO("Initializing np::config");

		nn::Result result;

		result = nn::fs::MountSaveData();

		if (result.IsFailure())
			if (result <= nn::fs::ResultNotFormatted() || result <= nn::fs::ResultBadFormat() ||
				result <= nn::fs::ResultVerificationFailed())
			{
				NN_LOG_WARN("Formatting save data (result: 0x%08X)", result.GetPrintableBits());

				result = nn::fs::FormatSaveData(1, 1);
				if (result.IsFailure())
				{
					NN_LOG_FATAL("Failed to format save data:");
					NN_DBG_PRINT_RESULT(result);

					return result;
				}

				result = nn::fs::MountSaveData();
				if (result.IsFailure())
				{
					NN_LOG_FATAL("Failed to mount save data:");
					NN_DBG_PRINT_RESULT(result);

					return result;
				}
			}

		result = s_ConfigFile.TryInitialize(CONFIG_FILE_PATH, nn::fs::OPEN_MODE_CREATE | nn::fs::OPEN_MODE_READ | nn::fs::OPEN_MODE_WRITE);
		if (result.IsFailure())
		{
			NN_LOG_FATAL("Failed to initialize config file:");
			NN_DBG_PRINT_RESULT(result);

			return result;
		}

		s_Configuration = new Configuration;

		if (s_ConfigFile.GetSize() != sizeof(Configuration))
		{
			s_Configuration->show_help	   = true;	// default to showing help
			s_Configuration->last_location = -1;	// default to no location

			NN_LOG_INFO("Creating new configuration file, or overwriting existing one, s_Configuration = 0x%p", s_Configuration);

			result = s_ConfigFile.TrySetSize(sizeof(Configuration));
			if (result.IsFailure())
			{
				NN_LOG_FATAL("Failed to set size of config file:");
				NN_DBG_PRINT_RESULT(result);

				return result;
			}

			result = FlushConfiguration();
			if (result.IsFailure())
			{
				NN_LOG_FATAL("Failed to flush configuration:");
				NN_DBG_PRINT_RESULT(result);

				return result;
			}
		}
		else
		{
			NN_LOG_INFO("Reading configuration file, s_Configuration = 0x%p", s_Configuration);

			s32 bytesRead = 0;
			result		  = s_ConfigFile.TryRead(&bytesRead, s_Configuration, sizeof(Configuration));
			if (result.IsFailure())
			{
				NN_LOG_FATAL("Failed to read configuration file:");
				NN_DBG_PRINT_RESULT(result);

				return result;
			}
		}

		result = s_ConfigFile.TrySeek(0, nn::fs::POSITION_BASE_BEGIN);
		if (result.IsFailure())
		{
			NN_LOG_FATAL("Failed to seek to beginning of config file:");
			NN_DBG_PRINT_RESULT(result);

			return result;
		}

		ClearNonExistingTitles();

		return nn::ResultSuccess();
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

	Configuration* GetConfiguration(void)
	{
		return s_Configuration;
	}

	nn::Result FlushConfiguration(void)
	{
		NN_LOG_INFO("Flushing configuration to file");

		nn::Result result;

		s32 bytesWritten = 0;
		result			 = s_ConfigFile.TryWrite(&bytesWritten, s_Configuration, sizeof(Configuration), true);
		if (result.IsFailure())
		{
			NN_LOG_FATAL("Failed to write configuration to file:");
			NN_DBG_PRINT_RESULT(result);

			return result;
		}

		result = s_ConfigFile.TrySeek(0, nn::fs::POSITION_BASE_BEGIN);	// Ensure we are alway overwriting the file
		if (result.IsFailure())
		{
			NN_LOG_FATAL("Failed to seek to beginning of config file:");
			NN_DBG_PRINT_RESULT(result);

			return result;
		}

		return nn::ResultSuccess();
	}
}}	// namespace np::config
