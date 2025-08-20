/*!
* \file AllocCommon.cpp
*
* \author kechen
* \date 11/12/2018
*
*
*/
#include <stdint.h>
#include "AllocCommon.h"
#include "../AssertionMacros.h"

/*
==================
Mem_Alloc
==================
*/
void * Mem_Alloc16(const int size) {
	if (!size) {
		return nullptr;
	}
	const int paddedSize = (size + 15) & ~15;
	return AllocAligned(paddedSize, 16);
}

/*
==================
Mem_Free
==================
*/
void Mem_Free16(void *ptr) {
	if (ptr == nullptr) {
		return;
	}
	FreeAligned(ptr);
}

/*
==================
Mem_ClearedAlloc
==================
*/
void * Mem_ClearedAlloc(const int size)
{
	void * mem = Mem_Alloc16(size);
	memset(mem, 0, size);
	return mem;
}

/////////////////////////////////////////////////////////////////////////////////
// alignment related code adopted from "Game Engine Architecture 3rd: 6.2.1.3"
/////////////////////////////////////////////////////////////////////////////////

// Shift the given address upwards if/as necessary to
// ensure it is aligned to the given number of bytes.
FORCEINLINE uintptr_t AlignAddress(uintptr_t addr, size_t align)
{
	const size_t mask = align - 1;
	ASSERT((align & mask) == 0); // power of 2
	return (addr + mask) & ~mask;
}

// Shift the given pointer upwards if/as necessary to
// ensure it is aligned to the given number of bytes.
template<typename T>
T* AlignPointer(T* ptr, size_t align) {
	const uintptr_t addr = reinterpret_cast<uintptr_t>(ptr);
	const uintptr_t addrAligned = AlignAddress(addr, align);
	return reinterpret_cast<T*>(addrAligned);
}

void* AllocAligned(const int bytes, const int align) {
	// Allocate 'align' more bytes than we need.
	size_t actualBytes = bytes + align;
	// Allocate unaligned block.
	bzbee::uint8* pRawMem = new bzbee::uint8[actualBytes];
	// Align the block. If no alignment occurred,
	// shift it up the full 'align' bytes so we
	// always have room to store the shift.
	bzbee::uint8* pAlignedMem = AlignPointer(pRawMem, align);
	if (pAlignedMem == pRawMem)
		pAlignedMem += align;
	// Determine the shift, and store it.
	// (This works for up to 256-byte alignment.)
	ptrdiff_t shift = pAlignedMem - pRawMem;
	ASSERT(shift > 0 && shift <= 256);
	pAlignedMem[-1] = static_cast<bzbee::uint8>(shift & 0xFF);
	return pAlignedMem;
}

void FreeAligned(void* pMem) {
	if (pMem) {
		// Convert to U8 pointer.
		bzbee::uint8* pAlignedMem = reinterpret_cast<bzbee::uint8*>(pMem);
		// Extract the shift.
		ptrdiff_t shift = pAlignedMem[-1];
		if (shift == 0)
			shift = 256;
		// Back up to the actual allocated address,
		// and array-delete it.
		bzbee::uint8* pRawMem = pAlignedMem - shift;
		delete[] pRawMem;
	}
}