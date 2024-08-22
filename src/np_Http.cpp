#include <np/http.h>
#include <np/log.h>
#include <np/memory.h>

#include <nn/ac.h>
#include <nn/fs.h>
#include <nn/http.h>

#ifndef NN_BUILD_DEBUG
#	include <nn/am.h>
#	include <nn/crypto.h>
#	include <nn/nwm.h>

#	include <np/base64.h>
#endif

#include <np/log_Macros.h>

#define NP_HEADER_MAC_ADDRESS "3ds-mac"
#define NP_HEADER_NET_PASS_ID "3ds-nid"

namespace {
#ifdef NN_BUILD_DEBUG
// Citra will cycle a random MAC address every time it starts, so we need to set a static one for testing
char8		s_MacAddress[13] = "FFFFFFFFFFFF";
const char8 s_NetPassId[]	 = "AAAAAAAA";	// base64 encoded 8 bytes (all NULLs), this header is required but it isn't actually used
#else
char8 s_MacAddress[13];
char8 s_NetPassId[44];	// base64 encoded 32 bytes
#endif

nn::fnd::TimeSpan s_Timeout = nn::fnd::TimeSpan::FromSeconds(10);

void* s_HttpPostBuffer;

bool s_IsInitialized = false;
}  // namespace

namespace np { namespace http {

	namespace detail {
		nn::Result ConnectToAc()
		{
			NN_LOG_INFO("Connecting to AC");

			nn::Result result;

			// Create parameters for connection request
			nn::ac::Config config;

			result = nn::ac::CreateDefaultConfig(&config);
			if (result.IsFailure())
			{
				NN_PANIC_IF_FAILED(nn::ac::Finalize());

				return result;
			}

			// Issue connection request
			result = nn::ac::Connect(config);
			if (result.IsFailure())
				NN_PANIC_IF_FAILED(nn::ac::Finalize());

			return result;
		}

		nn::Result SendRequest(const char*					 url,
							   const nn::http::RequestMethod method,
							   const void*					 data,
							   size_t						 dataSize,
							   s32*							 statusCode,
							   u8*							 response,
							   size_t						 responseSize,
							   HttpHeader*					 additionalHeaders,
							   size_t						 headerCount)
		{
			NN_LOG_INFO("Sending request, url: %s, method: %d", url, method);

			using namespace nn::http;

			nn::Result result;
			bool	   cleanupStatus = false;

			if (!nn::ac::IsConnected())
			{
				NN_LOG_WARN("Not connected to AC, connecting.");

				result = ConnectToAc();
				if (result.IsFailure())
					return result;
			}

			RequestMethod methodCopy = method;
			if (!data || dataSize == 0)
			{
				NN_LOG_INFO("No data to send, checking method and changing if necessary.");

				if (method == REQUEST_METHOD_POST)
					methodCopy = REQUEST_METHOD_POST_NODATA;
				else if (method == REQUEST_METHOD_PUT)
					methodCopy = REQUEST_METHOD_PUT_NODATA;
			}

			Connection connection;
			result = connection.Initialize(url, methodCopy);
			if (result.IsFailure())
				return result;

			if (additionalHeaders && headerCount > 0)
			{
				NN_LOG_INFO("Adding additional headers.");

				for (size_t i = 0; i < headerCount; i++)
				{
					result = connection.AddHeaderField(additionalHeaders[i].key, additionalHeaders[i].value);

					if (result.IsFailure())
						goto CLEANUP;
				}
			}

			result = connection.AddHeaderField(NP_HEADER_MAC_ADDRESS, s_MacAddress);
			if (result.IsFailure())
				goto CLEANUP;

			result = connection.AddHeaderField(NP_HEADER_NET_PASS_ID, s_NetPassId);
			if (result.IsFailure())
				goto CLEANUP;

			// Only post and put requests have a body
			if (method == REQUEST_METHOD_POST || method == REQUEST_METHOD_PUT)
				if (data && dataSize > 0)
				{
					NN_LOG_INFO("Adding post data, size: %u", dataSize);

					result = connection.AddPostDataRaw(data, dataSize);
					if (result.IsFailure())
						goto CLEANUP;
				}

			result = connection.Connect();
			if (result.IsFailure())
				goto CLEANUP;

			if (!statusCode)
			{
				NN_LOG_WARN("No status code buffer provided, creating one.");

				statusCode	  = new s32;
				cleanupStatus = true;
			}

			result = connection.GetStatusCode(statusCode, s_Timeout);
			if (result.IsFailure())
				goto CLEANUP;

			if (*statusCode == -1)
			{
				NN_LOG_WARN("Status code is -1, connection timed out.");

				result = ResultTimeout();
				goto CLEANUP;
			}

			if (response != NULL && responseSize == 0)
			{
				NN_LOG_WARN(
					"Response buffer provided but size is 0, trying to determine size based on "
					"Content-Length header.");

				// Try to determine the size of the response based on the Content-Length header
				// If not present then set the response size to 0, the caller
				// will have to check if the response is empty
				// It will be up to the caller to free the response buffer
				size_t contentLengthStringSize = 0;
				result						   = connection.GetHeaderField("Content-Length", NULL, 0, s_Timeout, &contentLengthStringSize);
				if (result.IsSuccess() && contentLengthStringSize > 0)
				{
					char* contentLengthString = reinterpret_cast<char*>(std::malloc(contentLengthStringSize));
					connection.GetHeaderField("Content-Length", contentLengthString, contentLengthStringSize, s_Timeout);

					responseSize = std::atoi(contentLengthString);
					if (response)
						std::free(response);

					response = reinterpret_cast<u8*>(std::malloc(responseSize));
				}
			}

			if (response)
			{
				NN_LOG_INFO("Reading response, size: %u", responseSize);

				result = connection.Read(response, responseSize, s_Timeout);
				if (result.IsFailure())
					goto CLEANUP;
			}

CLEANUP:
			NN_LOG_INFO("Finalizing connection.");

			connection.Finalize();

			if (cleanupStatus)
				delete statusCode;

			return result;
		}
	}  // namespace detail

	void Initialize(void)
	{
		if (s_IsInitialized)
			return;

		NN_LOG_INFO("Initializing np::http");

		nn::Result result;

		NN_LOG_INFO("Initializing nn::ac.");

		result = nn::ac::Initialize();
		NN_PANIC_IF_FAILED(result);

		// Issue connection request
		result = detail::ConnectToAc();
		if (result.IsFailure())
		{
#ifndef NN_BUILD_DEBUG	// nn::ac::Connect will always error on Citra, even though they have internet, this just ensures that debug builds
						// can still connect
			NN_PANIC_IF_FAILED(result);
			return;
#endif
		}

#ifndef NN_BUILD_DEBUG

		NN_LOG_INFO("Getting MAC address.");

		nn::nwm::Mac mac;
		NN_PANIC_IF_FAILED(nn::nwm::GetMacAddress(mac));

		u8 macBuffer[nn::nwm::Mac::MAC_SIZE];
		mac.Get(macBuffer);

		nn::nstd::TSNPrintf(s_MacAddress,
							sizeof(s_MacAddress),
							"%02X%02X%02X%02X%02X%02X",
							macBuffer[0],
							macBuffer[1],
							macBuffer[2],
							macBuffer[3],
							macBuffer[4],
							macBuffer[5]);

		NN_PANIC_IF_FAILED(nn::am::InitializeForSystemMenu());

		bit32 deviceId;
		NN_PANIC_IF_FAILED(nn::am::GetDeviceId(&deviceId));

		NN_PANIC_IF_FAILED(nn::am::FinalizeForSystemMenu());

		NN_LOG_INFO("Device ID: %u", deviceId);

		char8 deviceIdHmacBuffer[nn::crypto::Sha256Context::HASH_SIZE];
		nn::crypto::CalculateHmacSha256(deviceIdHmacBuffer, &deviceId, sizeof(deviceId), macBuffer, sizeof(macBuffer));

		size_t size = sizeof(s_NetPassId);
		np::Base64Encode(s_NetPassId, &size, deviceIdHmacBuffer, sizeof(deviceIdHmacBuffer));
#endif

		NN_LOG_INFO("Initialized headers, MAC address: %s, NetPass ID: %s", s_MacAddress, s_NetPassId);

		NN_LOG_INFO("Initializing nn::http, post buffer size: %u", HTTP_POST_BUFFER_SIZE);

		s_HttpPostBuffer = std::malloc(HTTP_POST_BUFFER_SIZE + (HTTP_BUFFER_ALIGNMENT - 1));
		if (!s_HttpPostBuffer)
		{
			NN_PANIC("Failed to allocate HTTP post buffer");

			return;
		}

		s_HttpPostBuffer = reinterpret_cast<void*>(MEM_ROUNDUP(reinterpret_cast<u32>(s_HttpPostBuffer), HTTP_BUFFER_ALIGNMENT));

		NN_PANIC_IF_FAILED(nn::http::Initialize(reinterpret_cast<uptr>(s_HttpPostBuffer), HTTP_POST_BUFFER_SIZE));

		s_IsInitialized = true;
	}

	void Finalize(void)
	{
		if (!s_IsInitialized)
			return;

		NN_LOG_INFO("Finalizing np::http");

		NN_PANIC_IF_FAILED(nn::http::Finalize());

		NN_PANIC_IF_FAILED(nn::ac::Finalize());

		s_IsInitialized = false;
	}

	nn::Result Get(const char* url, u8* response, size_t responseSize, s32* statusCode, HttpHeader* additionalHeaders, size_t headerCount)
	{
		return detail::SendRequest(url,
								   nn::http::REQUEST_METHOD_GET,
								   NULL,
								   0,
								   statusCode,
								   response,
								   responseSize,
								   additionalHeaders,
								   headerCount);
	}

	nn::Result Post(const char* url,
					const void* data,
					size_t		dataSize,
					s32*		statusCode,
					u8*			response,
					size_t		responseSize,
					HttpHeader* additionalHeaders,
					size_t		headerCount)
	{
		return detail::SendRequest(url,
								   nn::http::REQUEST_METHOD_POST,
								   data,
								   dataSize,
								   statusCode,
								   response,
								   responseSize,
								   additionalHeaders,
								   headerCount);
	}

	nn::Result Put(const char* url,
				   const void* data,
				   size_t	   dataSize,
				   s32*		   statusCode,
				   u8*		   response,
				   size_t	   responseSize,
				   HttpHeader* additionalHeaders,
				   size_t	   headerCount)
	{
		return detail::SendRequest(url,
								   nn::http::REQUEST_METHOD_PUT,
								   data,
								   dataSize,
								   statusCode,
								   response,
								   responseSize,
								   additionalHeaders,
								   headerCount);
	}

	nn::Result Delete(const char* url,
					  s32*		  statusCode,
					  u8*		  response,
					  size_t	  responseSize,
					  HttpHeader* additionalHeaders,
					  size_t	  headerCount)
	{
		return detail::SendRequest(url,
								   nn::http::REQUEST_METHOD_DELETE,
								   NULL,
								   0,
								   statusCode,
								   response,
								   responseSize,
								   additionalHeaders,
								   headerCount);
	}

}}	// namespace np::http
