// This file is used to define logging macros for the project.
// Make sure that this is the last file included in a source file.

#if defined(NN_BUILD_RELEASE) && defined(NP_LOGGING_ENABLED)

#	undef NN_LOG_INFO
#	define NN_LOG_INFO(format, ...) NN_LOG("[INFO] " format " (%s:%d)\n", ##__VA_ARGS__, NN_FILE_NAME, __LINE__)

#	undef NN_LOG_WARN
#	define NN_LOG_WARN(format, ...) NN_LOG("[WARN] " format " (%s:%d)\n", ##__VA_ARGS__, NN_FILE_NAME, __LINE__)

#	undef NN_LOG_ERROR
#	define NN_LOG_ERROR(format, ...) NN_LOG("[ERROR] " format " (%s:%d)\n", ##__VA_ARGS__, NN_FILE_NAME, __LINE__)

#	undef NN_LOG_FATAL
#	define NN_LOG_FATAL(format, ...) NN_LOG("[FATAL] " format " (%s:%d)\n", ##__VA_ARGS__, NN_FILE_NAME, __LINE__)

#	undef NN_LOG_DEBUG
#	define NN_LOG_DEBUG(format, ...) NN_LOG("[DEBUG] " format " (%s:%d)\n", ##__VA_ARGS__, NN_FILE_NAME, __LINE__)

#endif
