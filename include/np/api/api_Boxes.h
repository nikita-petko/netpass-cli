#ifndef NP_API_API_BOX_H_
#define NP_API_API_BOX_H_

#include <nn/cec.h>

namespace np { namespace api {

	/*!
		@brief Starts the background thread for downloading messages.

		@return The result of the operation.
	*/
	nn::Result StartInboxDownloadThread(void);

	/*!
		@brief Finalizes the inbox download thread.
	*/
	void FinalizeInboxDownloadThread(void);

	/*!
		@brief Uploads all of the current outboxes to the server.

		@return The result of the operation.
	*/
	nn::Result UploadOutboxes(void);

	/*!
		@brief Triggers an immediate download of message boxes.
	*/
	void TriggerInboxDownload(void);

	/*!
		@brief Waits for the inbox download to complete.
		@warning This function will block until the inbox download is complete, so be careful when calling it.
	*/
	void WaitForInboxDownloadComplete(void);

}}	// namespace np::api

#endif	// NP_API_API_BOX_H_
