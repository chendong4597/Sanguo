/*!
* \file AllocCommon.h
*
* \author kechen
* \date 11/12/2018
*
*
*/
#pragma once

#include "../BaseType.h"


// memory tag names are used to sort allocations for sys_dumpMemory and other reporting functions
enum memTag_t {
#define MEM_TAG( x )	TAG_##x,
	MEM_TAG(IDLIB_LIST)
	MEM_TAG(JOBLIST)
	TAG_NUM_TAGS,
};

void *		Mem_Alloc16(const int size);
void		Mem_Free16(void *ptr);

FORCEINLINE void *	Mem_Alloc(const int size) { return Mem_Alloc16(size); }
FORCEINLINE void	Mem_Free(void *ptr) { Mem_Free16(ptr); }

void *				Mem_ClearedAlloc(const int size);

// Aligned allocation function. IMPORTANT: 'align' must be a power of 2 (typically 4, 8 or 16).
void *	AllocAligned(const int bytes, const int align);

void	FreeAligned(void* pMem);