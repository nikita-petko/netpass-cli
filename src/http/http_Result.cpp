#include <np/http/http_Result.h>

#include <nn/http.h>

namespace np { namespace http {

const char* ConvertResultToString(nn::http::ResultCode result)
{
    switch (result)
    {
        case nn::http::ER_NONE:
            return "No error.";
        case nn::http::ER_INVALID_STATUS:
            return "Invalid status.";
        case nn::http::ER_INVALID_PARAM:
            return "Illegal parameter.";
        case nn::http::ER_NOMEM:
            return "Dynamic memory allocation failure.";
        case nn::http::ER_CREATE_EVENT:
            return "Event generation failure.";
        case nn::http::ER_CREATE_MUTEX:
            return "Mutex creation failure.";
        case nn::http::ER_CREATE_QUEUE:
            return "Message queue creation failure.";
        case nn::http::ER_CREATE_THREAD:
            return "Thread creation failure.";
        case nn::http::ER_CONNECTION_NOT_INITIALIZED:
            return "Destination not allocated.";
        case nn::http::ER_ALREADY_ASSIGN_HOST:
            return "Destination already allocated.";
        case nn::http::ER_SESSION:
            return "The IPC session is invalid.";
        case nn::http::ER_CLIENT_PROCESS_MAX:
            return "A number of clients equivalent to the maximum number of simultaneous client processes is already being used.";
        case nn::http::ER_IPC_SESSION_MAX:
            return "The maximum number of simultaneous IPC session connections is already connected. (The number of clients and number of connections are both already at the maximum.)";
        case nn::http::ER_TIMEOUT:
            return "Timeout.";
        case nn::http::ER_ALREADY_INITIALIZED:
            return "Library already initialized.";
        case nn::http::ER_NOT_INITIALIZED:
            return "Library not yet initialized.";
        case nn::http::ER_MSGQ_SEND_LSN:
            return "Failure sending to the listener thread message queue.";
        case nn::http::ER_MSGQ_RECV_LSN:
            return "Failure receiving from the listener thread message queue.";
        case nn::http::ER_MSGQ_RECV_COMM:
            return "Failure receiving from the communication thread message queue.";
        case nn::http::ER_CONN_NOMORE:
            return "The maximum number of registerable connection handles was exceeded.";
        case nn::http::ER_CONN_NOSUCH:
            return "No such connection handle.";
        case nn::http::ER_CONN_STATUS:
            return "The connection handle status is invalid.";
        case nn::http::ER_CONN_ADD:
            return "Connection handle registration failure.";
        case nn::http::ER_CONN_CANCELED:
            return "The connection handle was canceled.";
        case nn::http::ER_CONN_HOST_MAX:
            return "Exceeded the maximum number of simultaneous connections to the same host.";
        case nn::http::ER_CONN_PROCESS_MAX:
            return "Exceeded the maximum number of connections used by one process.";
        case nn::http::ER_REQ_URL:
            return "Invalid URL.";
        case nn::http::ER_REQ_CONNMSG_PORT:
            return "Invalid CONNECT send port number.";
        case nn::http::ER_REQ_UNKNOWN_METHOD:
            return "Unknown method.";
        case nn::http::ER_RES_HEADER:
            return "Invalid HTTP header.";
        case nn::http::ER_RES_NONEWLINE:
            return "No newline character in the HTTP header.";
        case nn::http::ER_RES_BODYBUF:
            return "HTTP body receive buffer error.";
        case nn::http::ER_RES_BODYBUF_SHORTAGE:
            return "The HTTP body receive buffer is too small.";
        case nn::http::ER_POST_ADDED_ANOTHER:
            return "Failure adding to POST data. (A different POST type already exists.)";  
        case nn::http::ER_POST_BOUNDARY:
            return "Boundary cannot be properly set.";
        case nn::http::ER_POST_SEND:
            return "POST request send failure.";
        case nn::http::ER_POST_UNKNOWN_ENCTYPE:
            return "Unknown encoding type.";
        case nn::http::ER_POST_NODATA:
            return "Send data not set.";
        case nn::http::ER_SSL:
            return "SSL error.";
        case nn::http::ER_SSL_CERT_EXIST:
            return "The SSL certificate is already set. (It must be deleted before re-registering.)";
        case nn::http::ER_SSL_NO_CA_CERT_STORE:
            return "No such SSL CA certificate store is registered.";
        case nn::http::ER_SSL_NO_CLIENT_CERT:
            return "No such SSL client certificate is registered.";
        case nn::http::ER_SSL_CA_CERT_STORE_MAX:
            return "The maximum number of simultaneously registerable SSL per-process CA certificate stores is already registered.";
        case nn::http::ER_SSL_CLIENT_CERT_MAX:
            return "The maximum number of simultaneously registerable SSL per-process client certificates is already registered.";
        case nn::http::ER_SSL_FAIL_TO_CREATE_CERT_STORE:    
            return "Failed to create the SSL certificate store.";
        case nn::http::ER_SSL_FAIL_TO_CREATE_CLIENT_CERT:
            return "Failed to create the SSL client certificate.";
        case nn::http::ER_SSL_CRL_EXIST:
            return "The SSL CRL is already set. (It must be deleted before re-registering.)";
        case nn::http::ER_SSL_NO_CRL_STORE:
            return "No such SSL CRL store is registered.";
        case nn::http::ER_SSL_CRL_STORE_MAX:
            return "The maximum number of simultaneously registerable SSL per-process certificate stores is already registered.";
        case nn::http::ER_SSL_FAIL_TO_CREATE_CRL_STORE:
            return "Failed to create the SSL CRL store.";
        case nn::http::ER_SOC_DNS:
            return "DNS name resolution failure.";
        case nn::http::ER_SOC_SEND:
            return "Socket data send failure.";
        case nn::http::ER_SOC_RECV:
            return "Socket data receive failure.";
        case nn::http::ER_SOC_CONN:
            return "Socket connection failure.";
        case nn::http::ER_SOC_KEEPALIVE_DISCONNECTED:
            return "Keep-alive communications have been disconnected from the server side.";
        case nn::http::ER_GET_PROXY_SETTING:
            return "Failed to get the proxy value set by the device.";
        default:
            return "Unknown error.";
    }
}
}}
