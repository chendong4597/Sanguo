/*!
* \file BlockAllocator.h
* \BlockAllocator adopted from Doom3 Engine's idBlockAlloc
*
* \author kechen
* \date 11/12/2018
*
*
*/

#pragma once

#include "AllocCommon.h"
#include <AssertionMacros.h>

/*
===============================================================================

	Block based allocator for fixed size objects.

	All objects of the 'type' are properly constructed and destructed when reused.

===============================================================================
*/

#define BLOCK_ALLOC_ALIGNMENT 16
#define CONST_MAX( x, y )			( (x) > (y) ? (x) : (y) )

// Define this to force all block allocators to act like normal new/delete allocation
// for tool checking.
//#define	FORCE_DISCRETE_BLOCK_ALLOCS

template<class _type_, int _blockSize_>
class bzbBlockAlloc {
public:
	FORCEINLINE				bzbBlockAlloc();
	FORCEINLINE				~bzbBlockAlloc();

	// returns total size of allocated memory
	size_t					Allocated() const { return m_total * sizeof( _type_ ); }

	// returns total size of allocated memory including size of (*this)
	size_t					Size() const { return sizeof( *this ) + Allocated(); }

	FORCEINLINE void		Shutdown();
	FORCEINLINE void		SetFixedBlocks( int numBlocks );
	FORCEINLINE void		FreeEmptyBlocks();

	FORCEINLINE _type_ *	Alloc();
	FORCEINLINE void		Free( _type_ *element );

	int						GetTotalCount() const { return m_total; }
	int						GetAllocCount() const { return m_active; }
	int						GetFreeCount()  const { return m_total - m_active; }

private:
	union element_t {
		_type_ *		data;	// this is a hack to make sure the save game system marks _type_ as saveable
		element_t *		next;
		bzbee::uint8	buffer[(CONST_MAX( sizeof( _type_ ), sizeof( element_t * ) ) + ( BLOCK_ALLOC_ALIGNMENT - 1 ) ) & ~( BLOCK_ALLOC_ALIGNMENT - 1 )];
	};

	class bzbBlock {
	public:
		element_t		elements[_blockSize_];
		bzbBlock *		next;
		element_t *		free;		// list with free elements in this block (temp used only by FreeEmptyBlocks)
		int				freeCount;	// number of free elements in this block (temp used only by FreeEmptyBlocks)
	};

	bzbBlock *			m_pBlocks;
	element_t *			m_pFree;
	int					m_total;
	int					m_active;
	bool				m_bAllowAllocs;

	FORCEINLINE void	AllocNewBlock();
};

/*
========================
bzbBlockAlloc<_type_,_blockSize_,align_t>::bzbBlockAlloc
========================
*/
template<class _type_, int _blockSize_>
FORCEINLINE bzbBlockAlloc<_type_,_blockSize_>::bzbBlockAlloc() :
	m_pBlocks( nullptr ),
	m_pFree( nullptr ),
	m_total( 0 ),
	m_active( 0 ),
	m_bAllowAllocs( true )
{
}

/*
========================
bzbBlockAlloc<_type_,_blockSize__,align_t>::~bzbBlockAlloc
========================
*/
template<class _type_, int _blockSize_>
FORCEINLINE bzbBlockAlloc<_type_,_blockSize_>::~bzbBlockAlloc() {
	Shutdown();
}

/*
========================
bzbBlockAlloc<_type_,_blockSize_,align_t>::Alloc
========================
*/
template<class _type_, int _blockSize_>
FORCEINLINE _type_ * bzbBlockAlloc<_type_,_blockSize_>::Alloc() {
#ifdef FORCE_DISCRETE_BLOCK_ALLOCS
	// for debugging tools
	return new _type_;
#else
	if ( m_pFree == nullptr ) {
		if ( !m_bAllowAllocs ) {
			return nullptr;
		}
		AllocNewBlock();
	}

	m_active++;
	element_t * element = m_pFree;
	m_pFree = m_pFree->next;
	element->next = nullptr;

	_type_ * t = (_type_ *) element->buffer;
	new ( t ) _type_;
	return t;
#endif
}

/*
========================
bzbBlockAlloc<_type_,_blockSize_,align_t>::Free
========================
*/
template<class _type_, int _blockSize_>
FORCEINLINE void bzbBlockAlloc<_type_,_blockSize_>::Free( _type_ * t ) {
#ifdef FORCE_DISCRETE_BLOCK_ALLOCS
	// for debugging tools
	delete t;
#else
	if ( t == nullptr ) {
		return;
	}

	t->~_type_();

	element_t * element = (element_t *)( t );
	element->next = m_pFree;
	m_pFree = element;
	m_active--;
#endif
}

/*
========================
bzbBlockAlloc<_type_,_blockSize_,align_t>::Shutdown
========================
*/
template<class _type_, int _blockSize_>
FORCEINLINE void bzbBlockAlloc<_type_,_blockSize_>::Shutdown() {
	while( m_pBlocks != nullptr ) {
		bzbBlock * block = m_pBlocks;
		m_pBlocks = m_pBlocks->next;
		Mem_Free( block );
	}
	m_pBlocks = nullptr;
	m_pFree = nullptr;
	m_total = m_active = 0;
}

/*
========================
bzbBlockAlloc<_type_,_blockSize_,align_t>::SetFixedBlocks
========================
*/
template<class _type_, int _blockSize_>
FORCEINLINE void bzbBlockAlloc<_type_,_blockSize_>::SetFixedBlocks( int numBlocks ) {
	int currentNumBlocks = 0;
	for ( bzbBlock * block = m_pBlocks; block != nullptr; block = block->next ) {
		currentNumBlocks++;
	}
	for ( int i = currentNumBlocks; i < numBlocks; i++ ) {
		AllocNewBlock();
	}
	m_bAllowAllocs = false;
}

/*
========================
bzbBlockAlloc<_type_,_blockSize_,align_t>::AllocNewBlock
========================
*/
template<class _type_, int _blockSize_>
FORCEINLINE void bzbBlockAlloc<_type_,_blockSize_>::AllocNewBlock() {
	bzbBlock * block = (bzbBlock *)Mem_Alloc( sizeof( bzbBlock ));
	block->next = m_pBlocks;
	m_pBlocks = block;
	for ( int i = 0; i < _blockSize_; i++ ) {
		block->elements[i].next = m_pFree;
		m_pFree = &block->elements[i];
		ASSERT( ( ( (uintptr_t)m_pFree ) & ( BLOCK_ALLOC_ALIGNMENT - 1 ) ) == 0 );
	}
	m_total += _blockSize_;
}

/*
========================
bzbBlockAlloc<_type_,_blockSize_,align_t>::FreeEmptyBlocks
========================
*/
template<class _type_, int _blockSize_>
FORCEINLINE void bzbBlockAlloc<_type_,_blockSize_>::FreeEmptyBlocks() {
	// first count how many free elements are in each block
	// and build up a free chain per block
	for ( bzbBlock * block = m_pBlocks; block != nullptr; block = block->next ) {
		block->free = nullptr;
		block->freeCount = 0;
	}
	for ( element_t * element = m_pFree; element != nullptr; ) {
		element_t * next = element->next;
		for ( bzbBlock * block = m_pBlocks; block != nullptr; block = block->next ) {
			if ( element >= block->elements && element < block->elements + _blockSize_ ) {
				element->next = block->free;
				block->free = element;
				block->freeCount++;
				break;
			}
		}
		// if this assert fires, we couldn't find the element in any block
		ASSERT( element->next != next );
		element = next;
	}
	// now free all blocks whose free count == _blockSize_
	bzbBlock * prevBlock = nullptr;
	for ( bzbBlock * block = m_pBlocks; block != nullptr; ) {
		bzbBlock * next = block->next;
		if ( block->freeCount == _blockSize_ ) {
			if ( prevBlock == nullptr ) {
				ASSERT( m_pBlocks == block );
				m_pBlocks = block->next;
			} else {
				ASSERT( prevBlock->next == block );
				prevBlock->next = block->next;
			}
			Mem_Free( block );
			m_total -= _blockSize_;
		} else {
			prevBlock = block;
		}
		block = next;
	}
	// now rebuild the free chain
	m_pFree = nullptr;
	for ( bzbBlock * block = m_pBlocks; block != nullptr; block = block->next ) {
		for ( element_t * element = block->free; element != nullptr; ) {
			element_t * next = element->next;
			element->next = m_pFree;
			m_pFree = element;
			element = next;
		}
	}
}
