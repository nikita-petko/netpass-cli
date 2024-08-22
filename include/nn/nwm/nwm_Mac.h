#ifndef NN_NWM_NWM_MAC_H_
#define NN_NWM_NWM_MAC_H_

#include <nn/assert.h>
#include <nn/nstd.h>
#include <nn/types.h>

#define NN_NWM_MAC_SIZE 6

#ifdef __cplusplus

#	include <cstdio>
#	include <cstring>

namespace nn { namespace nwm {
	class Mac
	{
	public:
		static const size_t MAC_SIZE = NN_NWM_MAC_SIZE;

	private:
		bit8 m_mac[MAC_SIZE];
		bit8 m_pad_[2];

	public:
		Mac() { std::memset(&m_mac[0], 0, MAC_SIZE); };
		~Mac() {}

		inline void Get(bit8 outputMac[MAC_SIZE]) const { std::memcpy(outputMac, m_mac, MAC_SIZE); }
	};

}}	// namespace nn::nwm

#endif	// __cplusplus

#endif /* NN_NWM_NWM_MAC_H_ */
