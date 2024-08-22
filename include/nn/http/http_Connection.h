#ifndef NN_HTTP_HTTP_CONNECTION_H_
#define NN_HTTP_HTTP_CONNECTION_H_

#include <nn/Result.h>
#include <nn/Handle.h>
#include <nn/http/http_Types.h>
#include <nn/http/http_Result.h>
#include <nn/http/http_ConnectionIpc.h>
#include <nn/http/http_CertStore.h>
#include <nn/http/http_ClientCert.h>
#include <nn/util/util_NonCopyable.h>
#include <nn/fnd/fnd_TimeSpan.h>

#ifdef __cplusplus

namespace nn {
namespace http {

class Connection : private nn::util::NonCopyable<Connection>
{
public:
    explicit Connection(void);
    explicit Connection(const char* pUrl, RequestMethod method = REQUEST_METHOD_GET, bool isUseDefaultProxy = true);
    virtual ~Connection(void);

    nn::Result Initialize(const char* pUrl, RequestMethod method = REQUEST_METHOD_GET, bool isUseDefaultProxy = true);
    nn::Result Finalize(void);

    nn::Result SetProxy(const char* pProxyName, u16 port, const char* pUserName, const char* pPassword);
    nn::Result SetBasicAuthorization(const char* pUserName, const char* pPassword);

    nn::Result Connect(void);
    nn::Result ConnectAsync(void);
    nn::Result Cancel(void);
    nn::Result GetStatus(Status* pStatusBuf) const;
    nn::Result GetError(ResultCode* pResultCodeBuf) const;
    nn::Result GetProgress(size_t* pReceivedLen, size_t* pContentLen) const;
    nn::Result Read(u8* pBodyBuf, size_t bufLen);
    nn::Result Read(u8* pBodyBuf, size_t bufLen, const nn::fnd::TimeSpan& timeout);
    nn::Result GetHeaderField(const char* pLabel, char* pFieldBuf, size_t bufSize, size_t* pFieldLengthCourier = NULL) const;
    nn::Result GetHeaderField(const char* pLabel, char* pFieldBuf, size_t bufSize, const nn::fnd::TimeSpan& timeout, size_t* pFieldLengthCourier = NULL) const;
    nn::Result GetHeaderAll(char* pHeaderBuf, size_t bufSize, size_t* pLengthCourier = NULL) const;
    nn::Result GetHeaderAll(char* pHeaderBuf, size_t bufSize, const nn::fnd::TimeSpan& timeout, size_t* pLengthCourier = NULL) const;
    nn::Result GetStatusCode(s32* pStatusCodeCourier) const;
    nn::Result GetStatusCode(s32* pStatusCodeCourier, const nn::fnd::TimeSpan& timeout) const;

    nn::Result AddHeaderField(const char* pLabel, const char* pValue);
    nn::Result AddPostDataAscii(const char* pLabel, const char* pValue);
    nn::Result AddPostDataBinary(const char* pLabel, const void* pValue, size_t valueSize);
    nn::Result AddPostDataRaw(const void* pValue, size_t valueSize);
    nn::Result SetLazyPostDataSetting( PostDataType dataType );
    nn::Result SetLazyPostDataSetting( PostDataType dataType, size_t dataSize );
    nn::Result NotifyFinishSendPostData( void );
    nn::Result SetPostDataEncoding(EncodingType type);
    nn::Result SendPostDataAscii(const char* pLabel, const char* pValue);
    nn::Result SendPostDataAscii(const char* pLabel, const char* pValue, const nn::fnd::TimeSpan& timeout);
    nn::Result SendPostDataBinary(const char* pLabel, const void* pValue, size_t valueSize);
    nn::Result SendPostDataBinary(const char* pLabel, const void* pValue, size_t valueSize, const nn::fnd::TimeSpan& timeout);
    nn::Result SendPostDataRaw(const void* pValue, size_t valueSize);
    nn::Result SendPostDataRaw(const void* pValue, size_t valueSize, const nn::fnd::TimeSpan& timeout);

    nn::Result SetRootCa(const u8* pCertData, size_t certDataSize);
    nn::Result SetRootCa( InternalCaCertId inCaCertName );
    nn::Result SetRootCaStore(CertStore& certStore);
    nn::Result SetClientCert(const u8* pCertData, size_t certDataSize, const u8* pPrivateKeyData, size_t privateKeyDataSize);
    nn::Result SetClientCert( InternalClientCertId inClientCertName );
    nn::Result SetClientCert(ClientCert& clientCert);
    nn::Result GetSslError(s32* pResultCodeBuf) const;
    nn::Result SetVerifyOption(u32 verifyOption);
    nn::Result DisableVerifyOptionForDebug( u32 excludeVerifyOptions );
    nn::Result SetRootCaDefault( void ) NN_ATTRIBUTE_DEPRECATED;
    nn::Result SetClientCertDefault( void ) NN_ATTRIBUTE_DEPRECATED;
    nn::Result SetKeepAliveAvailability( bool isUseKeepAlive );

private:
    ConnectionHandle m_ConnectHandle;
    nn::Handle m_privateIpcSession;
    mutable ConnectionIpc m_PrivateIpcClient;
    nn::Result SetProxyDefault(void);

    bool IsConnected() const
    {
        return (m_ConnectHandle > 0) ? true : false;
    }
    Result AssignPrivateIpcClient();
};


} // end of namespace http
} // end of namespace nn


#endif // __cplusplus



#include <nn/util/detail/util_CLibImpl.h>

NN_UTIL_DETAIL_CLIBIMPL_DEFINE_BUFFER_CLASS(nnhttpConnection, nn::http::Connection, 16, u32);

NN_EXTERN_C nnResult nnhttpConnectionInitialize(nnhttpConnection* this_, const char *pUrl, nnHttpRequestMethod method);
NN_EXTERN_C nnResult nnhttpConnectionFinalize(nnhttpConnection* this_);
NN_EXTERN_C nnResult nnhttpConnectionConnect(nnhttpConnection* this_);
NN_EXTERN_C nnResult nnhttpConnectionConnectAsync(nnhttpConnection* this_);
NN_EXTERN_C nnResult nnhttpConnectionCancel(nnhttpConnection* this_);
NN_EXTERN_C nnResult nnhttpConnectionRead(nnhttpConnection* this_, u8* pBodyBuf, u32 bufLen);
NN_EXTERN_C nnResult nnhttpConnectionGetStatus(const nnhttpConnection* this_, nnHttpStatus* pStatusBuf);
NN_EXTERN_C nnResult nnhttpConnectionGetProgress(const nnhttpConnection* this_, u32* pReceivedLen, u32* pContentLen);
NN_EXTERN_C nnResult nnhttpConnectionGetError(const nnhttpConnection* this_, nnHttpResultCode* pResultCodeBuf);

NN_EXTERN_C nnResult nnhttpConnectionSetProxy(nnhttpConnection* this_, const char* pProxyName, u16 port, const char* pUserName, const char* pPassword);
NN_EXTERN_C nnResult nnhttpConnectionSetBasicAuthorization(nnhttpConnection* this_, const char *pUserName, const char *pPassword);

NN_EXTERN_C nnResult nnhttpConnectionAddHeaderField(nnhttpConnection* this_, const char* pLabel, const char* pValue);
NN_EXTERN_C nnResult nnhttpConnectionAddPostDataAscii(nnhttpConnection* this_, const char* pLabel, const char* pValue);
NN_EXTERN_C nnResult nnhttpConnectionAddPostDataBinary(nnhttpConnection* this_, const char* pLabel, const char* pValue, u32 length);
NN_EXTERN_C nnResult nnhttpConnectionAddPostDataRaw(nnhttpConnection* this_, const char* pValue, u32 length);
NN_EXTERN_C nnResult nnhttpConnectionSetLazyPostDataSetting(nnhttpConnection* this_, nnHttpPostDataType dataType);
NN_EXTERN_C nnResult nnhttpConnectionNotifyFinishSendPostData(nnhttpConnection* this_);
NN_EXTERN_C nnResult nnhttpConnectionSetPostDataEncoding(nnhttpConnection* this_, nnHttpEncodingType type);
NN_EXTERN_C nnResult nnhttpConnectionSendPostDataAscii(nnhttpConnection* this_, const char* pLabel, const char* pValue);
NN_EXTERN_C nnResult nnhttpConnectionSendPostDataBinary(nnhttpConnection* this_, const char* pLabel, const void* pValue, size_t length);
NN_EXTERN_C nnResult nnhttpConnectionSendPostDataRaw(nnhttpConnection* this_, const void* pValue, u32 length);

NN_EXTERN_C nnResult nnhttpConnectionGetHeaderField(const nnhttpConnection* this_, const char* pLabel, char* pFieldBuf, size_t bufSize, size_t* pFieldLengthCourier);
NN_EXTERN_C nnResult nnhttpConnectionGetHeaderAll(const nnhttpConnection* this_, char* pHeaderBuf, size_t bufSize, size_t* pLengthCourier);
NN_EXTERN_C nnResult nnhttpConnectionGetStatusCode(const nnhttpConnection* this_, s32* pStatusCodeCourier);
NN_EXTERN_C nnResult nnhttpConnectionGetSslError(const nnhttpConnection* this_, s32* pResultCodeBuf);
NN_EXTERN_C nnResult nnhttpConnectionSetVerifyOption(nnhttpConnection* this_, u32 verifyOption);
NN_EXTERN_C nnResult nnhttpSetRootCa(nnhttpConnection* this_, const u8 *pCertData, size_t certDataSize);
NN_EXTERN_C nnResult nnhttpSetInternalRootCa( nnhttpConnection* this_, NnHttpInternalCaCertId inCaCertName );
NN_EXTERN_C nnResult nnhttpSetRootCaStore( nnhttpConnection* this_, nnhttpCertStore* pCertStore);
NN_EXTERN_C nnResult nnhttpSetClientCert(nnhttpConnection* this_, const u8* pCertData, size_t certDataSize, const u8* pPrivateKeyData, size_t privateKeyDataSize);
NN_EXTERN_C nnResult nnhttpSetInternalClientCert(nnhttpConnection* this_, NnHttpInternalClientCertId inClientCertName );
NN_EXTERN_C nnResult nnhttpSetClientCertObj(nnhttpConnection* this_, nnhttpClientCert* pClientCert);
NN_EXTERN_C nnResult nnhttpDisableVerifyOptionForDebug(nnhttpConnection* this_, u32 verifyOption);
NN_EXTERN_C nnResult nnhttpConnectionSetLazyPostDataSettingWithSize(nnhttpConnection* this_, nnHttpPostDataType dataType, size_t dataSize);

#endif /* NN_HTTP_HTTP_CONNECTION_H_ */
