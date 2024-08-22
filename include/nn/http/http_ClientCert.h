#ifndef NN_HTTP_HTTP_CLIENTCERT_H_
#define NN_HTTP_HTTP_CLIENTCERT_H_

#include <nn/util/util_NonCopyable.h>
#include <nn/http/http_Types.h>

#ifdef __cplusplus

namespace nn {
namespace http {

class Connection;

class ClientCert : private nn::util::NonCopyable<ClientCert>
{
    friend class Connection;
public:
    explicit ClientCert();
    virtual ~ClientCert (void);

    nn::Result Initialize(const u8* pCertData, size_t certDataSize, const u8* pPrivateKeyData, size_t privateKeyDataSize);
    nn::Result Initialize(InternalClientCertId inClientCertName );
    nn::Result Finalize(void);

private:
    bool m_isInitialized;
    NN_PADDING3;
    CertId m_certId;

    bool IsValid(){return m_isInitialized;}
};

} // end of namespace http
} // end of namespace nn

#endif // __cplusplus


#include <nn/util/detail/util_CLibImpl.h>


NN_UTIL_DETAIL_CLIBIMPL_DEFINE_BUFFER_CLASS(nnhttpClientCert, nn::http::ClientCert, 12, u32);

NN_EXTERN_C nnResult nnhttpClientCertInitialize(nnhttpClientCert* this_, const u8* pCertData, size_t certDataSize, const u8* pPrivateKeyData, size_t privateKeyDataSize);
NN_EXTERN_C nnResult nnhttpClientCertInitializeByInternalCert(nnhttpClientCert* this_, NnHttpInternalClientCertId inClientCertName);
NN_EXTERN_C nnResult nnhttpClientCertFinalize(nnhttpClientCert* this_);

#endif /* NN_HTTP_HTTP_CLIENTCERT_H_ */

