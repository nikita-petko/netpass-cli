#ifdef NP_LOGGING_ENABLED

#	include <nn/fs.h>
#	include <nn/dbg.h>
#	include <nn/nstd.h>

#	include <np/log.h>

#	define PRIVATE_DIR	  "sdmcwo:/private/"
#	define NIN3DS_DIR	  PRIVATE_DIR "Nintendo 3DS/"
#	define APP_DIR		  NIN3DS_DIR "app/"
#	define LOG_DIR		  APP_DIR "00F80800/"
#	define LOG_FILE_NAME LOG_DIR "%s_log.log"

namespace {

nn::fs::FileOutputStream s_LogFile;
nn::os::CriticalSection	 s_LogCriticalSection;

bool s_IsInitialized = false;

nn::Result
EnsureLogDirectoryExists(void)
{
	nn::Result result;

	// Check sdmcwo:/private/
	result = nn::fs::TryCreateDirectory(PRIVATE_DIR);
	if (result.IsFailure() && !nn::fs::ResultAlreadyExists::Includes(result))
		return result;

	// Check sdmcwo:/private/Nintendo 3DS/
	result = nn::fs::TryCreateDirectory(NIN3DS_DIR);
	if (result.IsFailure() && !nn::fs::ResultAlreadyExists::Includes(result))
		return result;

	// Check sdmcwo:/private/Nintendo 3DS/app/
	result = nn::fs::TryCreateDirectory(APP_DIR);
	if (result.IsFailure() && !nn::fs::ResultAlreadyExists::Includes(result))
		return result;

	// Check sdmcwo:/private/Nintendo 3DS/app/00F80800/
	result = nn::fs::TryCreateDirectory(LOG_DIR);
	if (result.IsFailure() && !nn::fs::ResultAlreadyExists::Includes(result))
		return result;

	return nn::ResultSuccess();
}

void
ToIso8601(nn::fnd::DateTimeParameters& parameters, char* buffer, size_t bufferSize)
{
	nn::nstd::TSNPrintf(buffer,
						bufferSize,
						"%04d%02d%02dT%02d%02d%02d",
						parameters.year,
						parameters.month,
						parameters.day,
						parameters.hour,
						parameters.minute,
						parameters.second);

	buffer[bufferSize - 1] = '\0';
}

}  // namespace

namespace np { namespace log {

	void Initialize(void)
	{
		if (s_IsInitialized)
			return;

		nn::Result result;

		result = nn::fs::MountSdmcWriteOnly();
		if (result.IsFailure())
		{
			NN_ERR_THROW_FATAL_ALL(result);

			return;
		}

		result = ::EnsureLogDirectoryExists();
		if (result.IsFailure())
		{
			NN_ERR_THROW_FATAL_ALL(result);

			return;
		}

		char logFileName[256];
		char iso8601[32];
		nn::fnd::DateTimeParameters dateTime = nn::fnd::DateTime::GetNow().GetParameters();
		::ToIso8601(dateTime, iso8601, sizeof(iso8601));
		nn::nstd::TSNPrintf(logFileName, sizeof(logFileName), LOG_FILE_NAME, iso8601);

		result = s_LogFile.TryInitialize(logFileName, true);
		if (result.IsFailure())
		{
			NN_ERR_THROW_FATAL_ALL(result);

			return;
		}

		// If it exists, seek to the end of the file
		result = s_LogFile.TrySeek(0, nn::fs::POSITION_BASE_END);
		if (result.IsFailure())
		{
			NN_ERR_THROW_FATAL_ALL(result);

			return;
		}

		// Write a newline to separate the log entries
		if (s_LogFile.GetSize() > 0)
		{
			s32 written = 0;
			result		= s_LogFile.TryWrite(&written, "\n", 1, true);
			if (result.IsFailure())
			{
				NN_LOG_ERROR("Failed to write newline to log file:");
				NN_DBG_PRINT_RESULT(result);

				return;
			}
		}

		s_LogCriticalSection.Initialize();

		s_IsInitialized = true;

		NN_LOG_INFO("Initialized np::log");
	}

	void Finalize(void)
	{
		NN_LOG_INFO("Finalizing np::log");

		s_LogFile.Finalize();
		s_LogCriticalSection.Finalize();

		nn::fs::Unmount("sdmcwo:");
	}

}}	// namespace np::log

namespace nn { namespace dbg { namespace detail {
	void PutString(const char* text, s32 length)
	{
		nn::os::CriticalSection::ScopedLock lock(s_LogCriticalSection);

		if (!::s_IsInitialized)
			return;

		if (!text || length <= 0)
			return;

		s32 written = 0;
		s_LogFile.TryWrite(&written, text, length, true);
	}
}}}	 // namespace nn::dbg::detail

#endif
