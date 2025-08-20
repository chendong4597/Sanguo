/*!
 * \file ChunckedArray.h
 *
 * \author mattho
 * \date 2016/10/18
 *
 *
 */
#pragma once

#include "IndirectArray.h"

NS_BEGIN

//////////////////////////////////////////////////////////////////////////
namespace detail
{
	template<typename ChunkType, typename ElementType, std::size_t NumElementsPerChunk>
	class ChunkedArrayIterator
	{
	public:
		typedef std::ptrdiff_t	difference_type;
		typedef ElementType		value_type;
		typedef ElementType*	pointer;
		typedef ElementType&	reference;
//		typedef std::random_access_iterator_tag iterator_category;

		ChunkedArrayIterator(ChunkType** inChunk, ElementType* inElement)
			: m_chunk(inChunk)
			, m_element(inElement)
		{}

		FORCEINLINE ElementType& operator*() const
		{
			return *m_element;
		}

		ChunkedArrayIterator& operator++()
		{
			++m_element;
			if (m_element == (*m_chunk)->data() + NumElementsPerChunk)
			{
				++m_chunk;
				m_element = (*m_chunk)->data();
			}
			return *this;
		}

		FORCEINLINE friend bool operator!=(const ChunkedArrayIterator& lhs, const ChunkedArrayIterator& rhs)
		{
			return lhs.m_element != rhs.m_element;
		}

	private:
		ChunkType**		m_chunk;
		ElementType*	m_element;
	};
}
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
template<typename T, std::size_t tTargetBytesPerChunk = 16384>
class chunked_array
{
	//static const std::size_t	NumElementsPerChunk = (tTargetBytesPerChunk / sizeof(T));
	enum { NumElementsPerChunk = tTargetBytesPerChunk / sizeof(T) };

	class ElementsChunk
	{
	public:
		FORCEINLINE T* data() { return (T*)m_data; }
		FORCEINLINE const T* data() const { return (const T*)m_data; }

	private:
		uint8	m_data[NumElementsPerChunk * sizeof(T)];
	};
	typedef ElementsChunk	chunk_type;

public:
	typedef	std::size_t	size_type;
	typedef T			value_type;
	typedef T*			pointer;
	typedef const T*	const_pointer;
	typedef T&			reference;
	typedef const T&	const_reference;
	typedef indirect_array<chunk_type>	storage_type;

	typedef detail::ChunkedArrayIterator<chunk_type, value_type, NumElementsPerChunk>				iterator;
	typedef detail::ChunkedArrayIterator<const chunk_type, const value_type, NumElementsPerChunk>	const_iterator;

public:
	FORCEINLINE chunked_array()
	{}
	chunked_array(size_type inNumElements)
		: m_arrayNum(inNumElements)
	{
		const size_type nNumChunks = (m_arrayNum + NumElementsPerChunk - 1) / NumElementsPerChunk;

		m_storage.purge(nNumChunks);
		for (size_type nIndex = 0; nIndex < nNumChunks; ++nIndex)
		{
			m_storage.emplace_back();
		}
	}

	FORCEINLINE chunked_array(const chunked_array& inOther)
		: m_storage(inOther.m_storage)
		, m_arrayNum(inOther.m_arrayNum)
	{}

	chunked_array(chunked_array&& inOther)
	{
		moveOrCopy(*this, inOther);
	}

	~chunked_array()
	{
		destructElements();
	}

	chunked_array& operator=(const chunked_array& inOther)
	{
		if (this != &inOther)
		{
			m_storage = inOther.m_storage;
			m_arrayNum = inOther.m_arrayNum;
		}
		return *this;
	}

	FORCEINLINE chunked_array& operator=(chunked_array&& inOther)
	{
		if (this != &inOther)
		{
			moveOrCopy(*this, inOther);
		}
		return *this;
	}

	//// Element access
	FORCEINLINE reference operator[](size_type index)
	{
		return *getElement(index);
	}

	FORCEINLINE const_reference operator[](size_type index) const
	{
		return *getElement(index);
	}

	//// Iterators
	FORCEINLINE iterator begin()
	{
		chunk_type** chunkPtr = m_storage.data();
		return iterator(chunkPtr, chunkPtr ? (*chunkPtr)->data() : nullptr);
	}
	FORCEINLINE iterator end()
	{
		chunk_type** chunkPtr = m_storage.data() + (m_arrayNum / NumElementsPerChunk);
		return iterator(chunkPtr, chunkPtr ? (*chunkPtr)->data() + (m_arrayNum % NumElementsPerChunk) : nullptr);
	}
	FORCEINLINE const_iterator begin() const
	{
		const chunk_type** chunkPtr = m_storage.data();
		return const_iterator(chunkPtr, chunkPtr ? (*chunkPtr)->data() : nullptr);
	}
	FORCEINLINE const_iterator end() const
	{
		const chunk_type** chunkPtr = m_storage.data() + (m_arrayNum / NumElementsPerChunk);
		return const_iterator(chunkPtr, chunkPtr ? (*chunkPtr)->data() + (m_arrayNum % NumElementsPerChunk) : nullptr);
	}

	//// Capacity
	FORCEINLINE bool empty() const { return (m_arrayNum == 0); }
	FORCEINLINE size_type size() const { return m_arrayNum; }
	FORCEINLINE void reserve(size_type inNewCapacity) { m_storage.reserve((inNewCapacity + NumElementsPerChunk - 1) / NumElementsPerChunk); }
	FORCEINLINE void shrink_to_fit() { m_storage.shrink_to_fit(); }

	//// Modifiers
	FORCEINLINE size_type push_back(const value_type& inElement)
	{
		return emplace_back(inElement);
	}

	template<typename... ParamTypes>
	FORCEINLINE size_type emplace_back(ParamTypes&&... inArgs)
	{
		const size_type nIndex = m_arrayNum;
		if ((m_arrayNum % NumElementsPerChunk) == 0)
		{
			m_storage.emplace_back();
		}

		new(getElement(m_arrayNum)) value_type(std::forward<ParamTypes>(inArgs)...);
		++m_arrayNum;

		return nIndex;
	}

	FORCEINLINE void clear()
	{
		m_storage.clear();
		m_arrayNum = 0;
	}

	FORCEINLINE void purge(size_type inSlack = 0)
	{
		const size_type nNumChunks = (inSlack + NumElementsPerChunk - 1) / NumElementsPerChunk;
		m_storage.purge(nNumChunks);
		m_arrayNum = 0;
	}

private:
	FORCEINLINE pointer getElement(const size_type index)
	{
		const size_type nChunkIndex = index / NumElementsPerChunk;
		const size_type nElementIndex = index % NumElementsPerChunk;
		return m_storage[nChunkIndex].data() + nElementIndex;
	}

	FORCEINLINE const_pointer getElement(const size_type index) const
	{
		const size_type nChunkIndex = index / NumElementsPerChunk;
		const size_type nElementIndex = index % NumElementsPerChunk;
		return m_storage[nChunkIndex].data() + nElementIndex;
	}

	void destructElements()
	{
		size_type nNumChunks = m_storage.size();
		if (nNumChunks > 0)
		{
			const size_type nNumElements = m_arrayNum % NumElementsPerChunk;
			{
				--nNumChunks;
				detail::destructElements(m_storage[nNumChunks].data(), nNumElements);
			}

			while (nNumChunks)
			{
				--nNumChunks;
				detail::destructElements(m_storage[nNumChunks].data(), NumElementsPerChunk);
			}
		}
	}

	template<typename ArrayType>
	FORCEINLINE static typename std::enable_if<container_trait<ArrayType>::MoveWillEmptyContainer>::type moveOrCopy(ArrayType& inDestArray, ArrayType& inSrcArray)
	{
		inDestArray.m_storage = (storage_type&&)inSrcArray.m_storage;
		inDestArray.m_arrayNum = inSrcArray.m_arrayNum;
		inSrcArray.m_arrayNum = 0;
	}

	template<typename ArrayType>
	FORCEINLINE static typename std::enable_if<!container_trait<ArrayType>::MoveWillEmptyContainer>::type moveOrCopy(ArrayType& inDestArray, ArrayType& inSrcArray)
	{
		inDestArray = inSrcArray;
	}

protected:
	storage_type	m_storage;
	size_type		m_arrayNum{};
};
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
template<typename T, std::size_t tTargetBytesPerChunk>
class container_trait<chunked_array<T, tTargetBytesPerChunk>> : public container_trait_base<chunked_array<T, tTargetBytesPerChunk>>
{
public:
	static const bool MoveWillEmptyContainer = container_trait<typename chunked_array<T, tTargetBytesPerChunk>::storage_type>::MoveWillEmptyContainer;
};
//////////////////////////////////////////////////////////////////////////

NS_END
