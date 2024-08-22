#ifndef NN_HTTP_HTTP_TYPES_H_
#define NN_HTTP_HTTP_TYPES_H_

typedef u32 NnHttpCertId;
typedef u32 NnHttpInternalCaCertId;
typedef u32 NnHttpInternalClientCertId;

#ifdef __cplusplus

namespace nn {
namespace http {

typedef NnHttpCertId CertId;
typedef NnHttpInternalCaCertId InternalCaCertId;
typedef NnHttpInternalClientCertId InternalClientCertId;
typedef u32 CertStoreId;
typedef s32 ConnectionHandle;

} // end of namespace http
} // end of namespace nn

#endif /*__cplusplus*/



#endif /* NN_NHTTP_NHTTP_TYPES_H_ */
