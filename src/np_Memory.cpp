#include <np/log.h>
#include <np/memory.h>

#include <nn/os.h>
#include <np/log_Macros.h>

namespace {
nn::fnd::ExpHeap s_AppHeap;
}  // namespace

namespace np { namespace memory {

	void Initialize()
	{
		NN_LOG_INFO("Initializing np::memory");

		s_AppHeap.Initialize(nn::os::GetDeviceMemoryAddress(), nn::os::GetDeviceMemorySize());

		NN_LOG_INFO("Device memory address: 0x%08X, size: 0x%08X", nn::os::GetDeviceMemoryAddress(), nn::os::GetDeviceMemorySize());
	}

	void Finalize()
	{
		NN_LOG_INFO("Finalizing np::memory");

		s_AppHeap.Finalize();
	}

	void* Allocate(size_t byteSize, s32 alignment, bit8 groupId, nn::fnd::ExpHeapBase::AllocationMode mode, bool reuse)
	{
		NN_LOG_INFO("Allocating %d bytes with alignment %d", byteSize, alignment);

		return s_AppHeap.Allocate(byteSize, alignment, groupId, mode, reuse);
	}

	void Free(void* ptr)
	{
		NN_LOG_INFO("Freeing memory at 0x%08X", ptr);

		s_AppHeap.Free(ptr);
	}

}}	// namespace np::memory
