#ifndef NP_CONFIG_CONFIG_TYPES_H_
#define NP_CONFIG_CONFIG_TYPES_H_

#include <nn/cec.h>

namespace np { namespace config {

	/*!
		@brief Represents the configuration data.
	*/
	struct Configuration
	{
		s8	 last_location;	 // The last location the user was in, this is up casted to a s32 when sent to the server.
		bool show_help;		 // Whether or not to show the help screen on startup
		nn::cec::TitleId
			disabled_title_ids[nn::cec::MESSAGE_BOX_NUM_MAX / 2];  // The title IDs of the disabled titles, up to 12 can be disabled.
	};

}}	// namespace np::config

#endif	// NP_CONFIG_CONFIG_TYPES_H_
