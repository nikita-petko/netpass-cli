﻿#ifndef NN_HTTP_HTTP_CONST_H_
#define NN_HTTP_HTTP_CONST_H_

#include <nn/net/compatible/nhttp.h>

enum nnHttpRequestMethod
{
    NN_HTTP_REQUEST_METHOD_NONE     = NHTTP_REQMETHOD_NONE,             
    NN_HTTP_REQUEST_METHOD_GET      = NHTTP_REQMETHOD_GET,              
    NN_HTTP_REQUEST_METHOD_POST     = NHTTP_REQMETHOD_POST,             
    NN_HTTP_REQUEST_METHOD_HEAD     = NHTTP_REQMETHOD_HEAD,             
    NN_HTTP_REQUEST_METHOD_PUT      = NHTTP_REQMETHOD_PUT,              
    NN_HTTP_REQUEST_METHOD_DELETE   = NHTTP_REQMETHOD_DELETE,           
    NN_HTTP_REQUEST_METHOD_POST_NODATA = NHTTP_REQMETHOD_POST_NODATA,   
    NN_HTTP_REQUEST_METHOD_PUT_NODATA = NHTTP_REQMETHOD_PUT_NODATA      
};

enum nnHttpEncodingType
{
    NN_HTTP_ENCODING_TYPE_AUTO      = NHTTP_ENCODING_TYPE_AUTO,         
    NN_HTTP_ENCODING_TYPE_URL       = NHTTP_ENCODING_TYPE_URL,          
    NN_HTTP_ENCODING_TYPE_MULTIPART = NHTTP_ENCODING_TYPE_MULTIPART     
};

enum nnHttpPostDataType
{
    NN_HTTP_POST_DATA_TYPE_URLENCODE = NHTTP_POST_DATA_TYPE_URLENCODE,  
    NN_HTTP_POST_DATA_TYPE_MULTIPART = NHTTP_POST_DATA_TYPE_MULTIPART,  
    NN_HTTP_POST_DATA_TYPE_RAW       = NHTTP_POST_DATA_TYPE_RAW         
};

enum nnHttpStatus
{
    NN_HTTP_STATUS_CREATED          = NHTTP_CONNST_CREATED,             
    NN_HTTP_STATUS_INITIALIZED      = NHTTP_CONNST_INITIALIZED,         
    NN_HTTP_STATUS_ENQUEUED_LSN     = NHTTP_CONNST_ENQUEUED_LSN,        
    NN_HTTP_STATUS_IN_LSN           = NHTTP_CONNST_IN_LSN,              
    NN_HTTP_STATUS_ENQUEUED_COMM    = NHTTP_CONNST_ENQUEUED_COMM,       
    NN_HTTP_STATUS_CONNECTING       = NHTTP_CONNST_CONNECTING,          
    NN_HTTP_STATUS_SENDING          = NHTTP_CONNST_SENDING,             
    NN_HTTP_STATUS_RECEIVING_HEADER = NHTTP_CONNST_RECEIVING_HEADER,    
    NN_HTTP_STATUS_RECEIVING_BODY   = NHTTP_CONNST_RECEIVING_BODY,      
    NN_HTTP_STATUS_RECEIVED         = NHTTP_CONNST_RECEIVED,            
    NN_HTTP_STATUS_FINISHED         = NHTTP_CONNST_FINISHED             
};

#ifdef __cplusplus

namespace nn {
namespace http {

enum RequestMethod
{
    REQUEST_METHOD_NONE     = NN_HTTP_REQUEST_METHOD_NONE,             
    REQUEST_METHOD_GET      = NN_HTTP_REQUEST_METHOD_GET,              
    REQUEST_METHOD_POST     = NN_HTTP_REQUEST_METHOD_POST,             
    REQUEST_METHOD_HEAD     = NN_HTTP_REQUEST_METHOD_HEAD,             
    REQUEST_METHOD_PUT      = NN_HTTP_REQUEST_METHOD_PUT,              
    REQUEST_METHOD_DELETE   = NN_HTTP_REQUEST_METHOD_DELETE,           
    REQUEST_METHOD_POST_NODATA = NN_HTTP_REQUEST_METHOD_POST_NODATA,   
    REQUEST_METHOD_PUT_NODATA = NN_HTTP_REQUEST_METHOD_PUT_NODATA      
};

enum EncodingType
{
    ENCODING_TYPE_AUTO      = NN_HTTP_ENCODING_TYPE_AUTO,               
    ENCODING_TYPE_URL       = NN_HTTP_ENCODING_TYPE_URL,                
    ENCODING_TYPE_MULTIPART = NN_HTTP_ENCODING_TYPE_MULTIPART           
};

enum PostDataType
{
    POST_DATA_TYPE_URLENCODE = NN_HTTP_POST_DATA_TYPE_URLENCODE,        
    POST_DATA_TYPE_MULTIPART = NN_HTTP_POST_DATA_TYPE_MULTIPART,        
    POST_DATA_TYPE_RAW       = NN_HTTP_POST_DATA_TYPE_RAW               
};

enum Status
{
    STATUS_CREATED          = NN_HTTP_STATUS_CREATED,                   
    STATUS_INITIALIZED      = NN_HTTP_STATUS_INITIALIZED,               
    STATUS_ENQUEUED_LSN     = NN_HTTP_STATUS_ENQUEUED_LSN,              
    STATUS_IN_LSN           = NN_HTTP_STATUS_IN_LSN,                    
    STATUS_ENQUEUED_COMM    = NN_HTTP_STATUS_ENQUEUED_COMM,             
    STATUS_CONNECTING       = NN_HTTP_STATUS_CONNECTING,                
    STATUS_SENDING          = NN_HTTP_STATUS_SENDING,                   
    STATUS_RECEIVING_HEADER = NN_HTTP_STATUS_RECEIVING_HEADER,          
    STATUS_RECEIVING_BODY   = NN_HTTP_STATUS_RECEIVING_BODY,            
    STATUS_RECEIVED         = NN_HTTP_STATUS_RECEIVED,                  
    STATUS_FINISHED         = NN_HTTP_STATUS_FINISHED                   
};

enum ProxyAuthType
{
    PROXY_AUTH_TYPE_NONE, 
    PROXY_AUTH_TYPE_BASIC 
};


namespace detail
{
    enum ConnectionEvent
    {
        CONN_EV_NONE             = NHTTP_CONN_EV_NONE,                  
        CONN_EV_POST_SEND        = NHTTP_CONN_EV_POST_SEND,             
        CONN_EV_POST_SEND_ALL    = NHTTP_CONN_EV_POST_SEND_ALL,         
        CONN_EV_HEADER_RECV_DONE = NHTTP_CONN_EV_HEADER_RECV_DONE,      
        CONN_EV_BODY_RECV_FULL   = NHTTP_CONN_EV_BODY_RECV_FULL,        
        CONN_EV_BODY_RECV_DONE   = NHTTP_CONN_EV_BODY_RECV_DONE,        
        CONN_EV_COMPLETE         = NHTTP_CONN_EV_COMPLETE,              
        CONN_EV_MAX              = NHTTP_CONN_EV_MAX                    
    };
} // end of namespace detail


} // end of namespace http
} // end of namespace nn


#endif /*__cplusplus*/

#endif /* NN_HTTP_HTTP_CONST_H_ */
