#ifdef NP_LOGGING_ENABLED

#	include <np/log.h>
#	include <nn/fs.h>
#	include <nn/dbg.h>
#	include <nn/nstd.h>

#	include <np/log_Macros.h>

#	define LOG_FILE_NAME "sdmcwo:/np-log.txt"

namespace {

nn::fs::FileOutputStream s_LogFile;
nn::os::CriticalSection	 s_LogCriticalSection;

bool s_IsInitialized = false;

}  // namespace

namespace np { namespace log {

	void Initialize(void)
	{
		if (s_IsInitialized)
			return;

		nn::Result result;

		nn::fs::MountSdmcWriteOnly();  // Result not important, we can assume that the SD card is inserted

		result = s_LogFile.TryInitialize(LOG_FILE_NAME, true);

		if (result.IsFailure())
			return;

		// If it exists, seek to the end of the file
		s_LogFile.Seek(0, nn::fs::POSITION_BASE_END);

		// Write a newline to separate the log entries
		if (s_LogFile.GetSize() > 0)
		{
			s32 written = 0;
			s_LogFile.TryWrite(&written, "\n", 1, true);
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
