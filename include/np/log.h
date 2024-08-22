#ifndef NP_LOG_H_
#define NP_LOG_H_

#ifdef NP_LOGGING_ENABLED

namespace np { namespace log {
	/*!
		@brief Initializes the logging system.
	*/
	void Initialize(void);

	/*!
		@brief Finalizes the logging system.
	*/
	void Finalize(void);
}}	// namespace np::log

#endif

#endif	// NP_LOG_H_
