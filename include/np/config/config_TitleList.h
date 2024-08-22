#ifndef NP_CONFIG_CONFIG_TITLELIST_H_
#define NP_CONFIG_CONFIG_TITLELIST_H_

#include <nn/cec.h>

namespace np { namespace config {
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

#endif	// NP_CONFIG_CONFIG_TITLELIST_H_
