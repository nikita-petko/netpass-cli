#ifndef NP_HTTP_HTTP_API_H_
#define NP_HTTP_HTTP_API_H_

#include <nn/http.h>

#include <np/http/http_Types.h>

namespace np { namespace http {
	/*!
		@brief Initializes the HTTP library.

        @return The result of the operation.
        @retval ResultSuccess The operation was successful.
        @retval ResultHttpPostBufferAllocationFailed The HTTP post buffer could not be allocated.
        @retval Any other result code The operation failed.
	*/
	nn::Result Initialize(void);

	/*!
		@brief Finalizes the HTTP library.
	*/
	void Finalize(void);

	/*!
		@brief Performs a GET request to the specified URL.
		@note If you do not know the size of the response, you can pass in a NULL response buffer and a responseSize of 0
			  and it will be determined based on the Content-Length header of the response, if the header is not present
			  then the response will be empty and the response buffer will be NULL.

		@param[in] url URL to request.
		@param[out] response Buffer to store the response.
		@param[in] responseSize Size of the response buffer.
		@param[out] statusCode Status code of the response, optional.
		@param[in] additionalHeaders Additional headers to send with the request.
		@param[in] headerCount Number of additional headers.

		@return The result of the operation.
	*/
	nn::Result Get(const char* url,
				   u8*		   response			 = NULL,
				   size_t	   responseSize		 = 0,
				   s32*		   statusCode		 = NULL,
				   HttpHeader* additionalHeaders = NULL,
				   size_t	   headerCount		 = 0);

	/*!
		@brief Performs a POST request to the specified URL.
		@note Specify response as NULL if you don't care about the response.
		@note Specify data as NULL if you don't want to send any data, but still want to perform a POST request.
		@note If data is specified, it will be sent as raw bytes with the Content-Type set to application/binary.
		@note If you do not know the size of the response, you can pass in a NULL response buffer and a responseSize of 0
			  and it will be determined based on the Content-Length header of the response, if the header is not present
			  then the response will be empty and the response buffer will be NULL.

		@param[in] url URL to request.
		@param[in] data Data to send.
		@param[in] dataLength Length of the data to send.
		@param[out] statusCode Status code of the response, optional.
		@param[out] response Buffer to store the response.
		@param[in] responseSize Size of the response buffer.
		@param[in] additionalHeaders Additional headers to send with the request.
		@param[in] headerCount Number of additional headers.

		@return The result of the operation.
	*/
	nn::Result Post(const char* url,
					const void* data			  = NULL,
					size_t		dataLength		  = 0,
					s32*		statusCode		  = NULL,
					u8*			response		  = NULL,
					size_t		responseSize	  = 0,
					HttpHeader* additionalHeaders = NULL,
					size_t		headerCount		  = 0);

	/*!
		@brief Performs a PUT request to the specified URL.
		@note Specify response as NULL if you don't care about the response.
		@note Specify data as NULL and dataLength as zero if you don't want to send any data, but still want to perform a PUT request.
		@note If data is specified, it will be sent as raw bytes with the Content-Type set to application/binary.
		@note If you do not know the size of the response, you can pass in a NULL response buffer and a responseSize of 0
			  and it will be determined based on the Content-Length header of the response, if the header is not present
			  then the response will be empty and the response buffer will be NULL.

		@param[in] url URL to request.
		@param[in] data Data to send.
		@param[in] dataLength Length of the data to send.
		@param[out] statusCode Status code of the response, optional.
		@param[out] response Buffer to store the response.
		@param[in] responseSize Size of the response buffer.
		@param[in] additionalHeaders Additional headers to send with the request.
		@param[in] headerCount Number of additional headers.

		@return The result of the operation.
	*/
	nn::Result Put(const char* url,
				   const void* data				 = NULL,
				   size_t	   dataLength		 = 0,
				   s32*		   statusCode		 = NULL,
				   u8*		   response			 = NULL,
				   size_t	   responseSize		 = 0,
				   HttpHeader* additionalHeaders = NULL,
				   size_t	   headerCount		 = 0);

	/*!
		@brief Performs a DELETE request to the specified URL.
		@note Specify response as NULL if you don't care about the response.
		@note If you do not know the size of the response, you can pass in a NULL response buffer and a responseSize of 0
			  and it will be determined based on the Content-Length header of the response, if the header is not present
			  then the response will be empty and the response buffer will be NULL.

		@param[in] url URL to request.
		@param[out] statusCode Status code of the response, optional.
		@param[out] response Buffer to store the response.
		@param[in] responseSize Size of the response buffer.
		@param[in] additionalHeaders Additional headers to send with the request.
		@param[in] headerCount Number of additional headers.

		@return The result of the operation.
	*/
	nn::Result Delete(const char* url,
					  s32*		  statusCode		= NULL,
					  u8*		  response			= NULL,
					  size_t	  responseSize		= 0,
					  HttpHeader* additionalHeaders = NULL,
					  size_t	  headerCount		= 0);
}}	// namespace np::http

#endif	// NP_HTTP_HTTP_API_H_
