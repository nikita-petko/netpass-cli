#ifndef NN_HTTP_HTTP_CERTSTORE_H_
#define NN_HTTP_HTTP_CERTSTORE_H_

#include <nn/util/util_NonCopyable.h>
#include <nn/http/http_Types.h>

#ifdef __cplusplus

namespace nn {
namespace http {

class Connection;

class CertStore : private nn::util::NonCopyable<CertStore>
{
    friend class Connection;
public:
    explicit CertStore();
    virtual ~CertStore (void);

    nn::Result Initialize(void);
    nn::Result Finalize(void);

    nn::Result RegisterCert(const u8* pCertData, size_t certDataSize, CertId* pCertIdCourier=NULL);
    nn::Result RegisterCert(InternalCaCertId inCaCertName, CertId* pCertIdCourier=NULL);
    nn::Result UnRegisterCert(CertId certId);



private:
    bool m_isInitialized;
    NN_PADDING3;
    CertStoreId m_certStoreId;

    bool IsValid(){return m_isInitialized;}
};

} // end of namespace http
} // end of namespace nn

#endif // __cplusplus


#include <nn/util/detail/util_CLibImpl.h>


NN_UTIL_DETAIL_CLIBIMPL_DEFINE_BUFFER_CLASS(nnhttpCertStore, nn::http::CertStore, 12, u32);

NN_EXTERN_C nnResult nnhttpCertStoreInitialize(nnhttpCertStore* this_);
NN_EXTERN_C nnResult nnhttpCertStoreFinalize(nnhttpCertStore* this_);
NN_EXTERN_C nnResult nnhttpCertStoreRegisterCert(nnhttpCertStore* this_, const u8* pCertData, size_t certDataSize, NnHttpCertId* pCertIdCourier);
NN_EXTERN_C nnResult nnhttpCertStoreRegisterInternalCert(nnhttpCertStore* this_, NnHttpInternalCaCertId inCaCertName, NnHttpCertId* pCertIdCourier);
NN_EXTERN_C nnResult nnhttpCertStoreUnRegisterCert(nnhttpCertStore* this_,  NnHttpCertId certId);

#endif /* NN_HTTP_HTTP_CERTSTORE_H_ */
