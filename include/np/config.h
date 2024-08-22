#ifndef NP_CONFIG_H_
#define NP_CONFIG_H_

#include <nn/Result.h>
#include <nn/cec.h>

namespace np { namespace config {

	/*!
		@brief Initializes the configuration library.
	*/
	void Initialize(void);

	/*!
		@brief Finalizes the configuration library.
	*/
	void Finalize(void);

	/*!
		@brief Represents the configuration data.
	*/
	struct Configuration
	{
		s8				 last_location;			  // The last location the user was in, this is up casted to a s32 when sent to the server.
		bool			 show_help;				  // Whether or not to show the help screen on startup
		nn::cec::TitleId disabled_title_ids[12];  // The title IDs of the disabled titles, up to 12 can be disabled.
	};

	/*!
		@brief Gets the the static configuration data.

		@return The configuration data.
	*/
	Configuration& GetConfiguration(void);

	/*!
		@brief Flushes the configuration data to the file.
	*/
	void FlushConfiguration(void);

	/*!
		@brief Small helper to check if a title ID is ignored.

		@param[in] titleId  The title ID to check.

		@return True if the title ID is ignored, false otherwise.
	*/
	bool IsTitleIdIgnored(nn::cec::TitleId titleId);

	/*!
		@brief Helper for clearing ignored titles out of the message box list.

		@param[in,out] pIn The message box list to clear.
	*/
	void ClearIgnoredTitles(nn::cec::MessageBoxList* pIn);

	/*!
		@brief Helper for adding a title ID to the ignored list.

		@param[in] titleId The title ID to ignore.
	*/
	void AddIgnoredTitle(nn::cec::TitleId titleId);

	/*!
		@brief Helper for removing a title ID from the ignored list.

		@param[in] titleId The title ID to remove.
	*/
	void RemoveIgnoredTitle(nn::cec::TitleId titleId);
}}	// namespace np::config

#endif	// NP_CONFIG_H_
