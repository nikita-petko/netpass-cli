#ifndef NP_CONFIG_CONFIG_API_H_
#define NP_CONFIG_CONFIG_API_H_

#include <np/config/config_Types.h>

namespace np { namespace config {
	/*!
		@brief Initializes the configuration library.
	*/
	nn::Result Initialize(void);

	/*!
		@brief Finalizes the configuration library.
	*/
	void Finalize(void);

	/*!
		@brief Gets the the static configuration data.

		@return The configuration data.
	*/
	Configuration* GetConfiguration(void);

	/*!
		@brief Flushes the configuration data to the file.
	*/
	nn::Result FlushConfiguration(void);
}}	// namespace np::config

#endif	// NP_CONFIG_CONFIG_API_H_
