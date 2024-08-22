#ifndef NP_MEMORY_MEMORY_MACROS_H_
#define NP_MEMORY_MEMORY_MACROS_H_

/*!
	@brief Gets the size of an array.

	@param[in] array The array to get the size of.
*/
#define MEM_SIZEOF_ARRAY(array) sizeof(array) / sizeof(array[0])

/*!
	@brief Rounds a value up to the nearest multiple of a base.

	@param[in] x The value to round up.
	@param[in] base The base to round up to.

	@return The rounded up value.
*/
#define MEM_ROUNDUP(x, base) (((x) + ((base) - 1)) & ~((base) - 1))

#endif	// NP_MEMORY_MEMORY_MACROS_H_
