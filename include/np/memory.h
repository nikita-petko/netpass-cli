#ifndef NP_MEMORY_H_
#define NP_MEMORY_H_

#include <nn/fnd.h>

/*!
	@brief Gets the size of an array.

	@param[in] array The array to get the size of.
*/
#define SIZEOF_ARRAY(array) sizeof(array) / sizeof(array[0])

/*!
	@brief Rounds a value up to the nearest multiple of a base.

	@param[in] x The value to round up.
	@param[in] base The base to round up to.

	@return The rounded up value.
*/
#define MEM_ROUNDUP(x, base) (((x) + ((base) - 1)) & ~((base) - 1))

namespace np { namespace memory {

	/*!
		@brief Initializes the memory library.
	*/
	void Initialize();

	/*!
		@brief Finalizes the memory library.
	*/
	void Finalize();

	/*!
		@brief Allocates memory.

		@param[in] byteSize The size of the memory to allocate.
		@param[in] alignment The alignment of the memory to allocate.
		@param[in] groupId The group ID of the memory to allocate.
		@param[in] mode The allocation mode of the memory to allocate.
		@param[in] reuse Whether or not to reuse memory.

		@return The allocated memory.
	*/
	void* Allocate(size_t								byteSize,
				   s32									alignment = nn::fnd::ExpHeapBase::DEFAULT_ALIGNMENT,
				   bit8									groupId	  = 0,
				   nn::fnd::ExpHeapBase::AllocationMode mode	  = nn::fnd::ExpHeapBase::ALLOCATION_MODE_FIRST_FIT,
				   bool									reuse	  = false);

	/*!
		@brief Frees memory.

		@param[in] ptr The memory to free.
	*/
	void Free(void* ptr);

}}	// namespace np::memory

#endif	// NP_MEMORY_H_
