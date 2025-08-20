/*!
 * \file IndirectArray.h
 *
 * \author mattho
 * \date 2016/10/18
 *
 *
 */
#pragma once

#include "DynamicArray.h"

NS_BEGIN

//////////////////////////////////////////////////////////////////////////
namespace detail
{
	template<typename ElementType, typename IteratorType>
	class DereferencingIterator
	{
	public:
		explicit DereferencingIterator(IteratorType inIter)
			: m_iter(inIter)
		{}

		FORCEINLINE ElementType& operator*() const
		{
			return *(ElementType*)*m_iter;
		}

		FORCEINLINE DereferencingIterator& operator++()
		{
			++m_iter;
			return *this;
		}

	private:
		FORCEINLINE friend bool operator!=(const DereferencingIterator& lhs, const DereferencingIterator& rhs)
		{
			return (lhs.m_iter != rhs.m_iter);
		}

	private:
		IteratorType	m_iter;
	};
}
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
template<typename T, typename Allocator = HeapAllocatePolicy>
class indirect_array
{
public:
	typedef std::size_t	size_type;
	typedef	T			value_type;
	typedef T*			pointer;
	typedef const T*	const_pointer;
	typedef T&			reference;
	typedef const T&	const_reference;

	typedef dynamic_array<void*, Allocator>	storage_type;

	typedef detail::DereferencingIterator<value_type, typename storage_type::iterator>				iterator;
	typedef detail::DereferencingIterator<const value_type, typename storage_type::const_iterator>	const_iterator;

public:
	indirect_array()
	{}

	indirect_array(const indirect_array& inOther)
	{
		for (void* item : inOther.m_storage)
		{
			value_type* inElement = (value_type*)std::malloc(sizeof(value_type));
			new(inElement)value_type(*(value_type*)item);
			m_storage.emplace_back(inElement);
		}
	}

	FORCEINLINE indirect_array(indirect_array&& inOther)
		: m_storage(std::move(inOther.m_storage))
	{}

	~indirect_array()
	{
		destructAndFreeElements();
	}

	//// Element access
	FORCEINLINE reference operator[](size_type index)
	{
		return *(pointer)m_storage[index];
	}

	FORCEINLINE const_reference operator[](size_type index) const
	{
		return *(const_pointer)m_storage[index];
	}

	FORCEINLINE pointer* data() { return (pointer*)m_storage.data(); }
	FORCEINLINE const_pointer* data() const { return (const_pointer*)m_storage.data(); }

	//// Iterators
	FORCEINLINE iterator begin() { return iterator(m_storage.begin()); }
	FORCEINLINE iterator end() { return iterator(m_storage.end()); }
	FORCEINLINE const_iterator begin() const { return const_iterator(m_storage.begin()); }
	FORCEINLINE const_iterator end() const { return const_iterator(m_storage.end()); }

	//// Capacity
	FORCEINLINE bool empty() const { return m_storage.empty(); }
	FORCEINLINE size_type size() const { return m_storage.size(); }
	FORCEINLINE void reserve(size_type inNewCapacity) { m_storage.reserve(inNewCapacity); }

	//// Modifiers
	FORCEINLINE size_type uninitializedBack(pointer inItem)
	{
		return m_storage.emplace_back(inItem);
	}

	template<typename... ParamTypes>
	FORCEINLINE size_type emplace_back(ParamTypes&&... inArgs)
	{
		value_type* inElement = (value_type*)std::malloc(sizeof(value_type));
		new(inElement)value_type(std::forward<ParamTypes>(inArgs)...);
		return m_storage.emplace_back(inElement);
	}

	size_type removeAt(size_type inIndex, size_type inNumElements = 1, bool bAllowShrinking = false)
	{
		ASSERT(inIndex >= 0);
		ASSERT(inIndex <= m_storage.size());
		ASSERT(inIndex + inNumElements <= m_storage.size());
		pointer* inElement = data() + inIndex;
		for (size_type index = inNumElements; index; --index)
		{
			typedef T	IndirectArrayDestructElementTypeTypedef;
			(*inElement)->IndirectArrayDestructElementTypeTypedef::~IndirectArrayDestructElementTypeTypedef();
			std::free(*inElement);
			++inElement;
		}
		return m_storage.removeAt(inIndex, inNumElements, bAllowShrinking);
	}

	FORCEINLINE void clear()
	{
		destructAndFreeElements();
		m_storage.clear();
	}

	FORCEINLINE void purge(size_type inSlack = 0)
	{
		destructAndFreeElements();
		m_storage.purge(inSlack);
	}

	indirect_array& operator=(const indirect_array& inOther)
	{
		if (this != &inOther)
		{
			purge(inOther.size());
			for (void* item : inOther.m_storage)
			{
				value_type* inElement = (value_type*)std::malloc(sizeof(value_type));
				new(inElement)value_type(*(value_type*)item);
				m_storage.emplace_back(inElement);
			}
		}
		return *this;
	}

	FORCEINLINE indirect_array& operator=(indirect_array&& inOther)
	{
		m_storage = std::move(inOther.m_storage);
		return *this;
	}

private:
	void destructAndFreeElements()
	{
		pointer* inElement = data();
		for (size_type nIndex = m_storage.size(); nIndex; --nIndex)
		{
			typedef T	IndirectArrayDestructElementTypeTypedef;
			(*inElement)->IndirectArrayDestructElementTypeTypedef::~IndirectArrayDestructElementTypeTypedef();
			std::free(*inElement);
			++inElement;
		}
	}

protected:
	storage_type	m_storage;
};
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
template<typename T, std::size_t tInlineElements, typename SecondaryPolicy = HeapAllocatePolicy>
using inline_indirect_array = indirect_array<T, InlineAllocatePolicy<tInlineElements, SecondaryPolicy> >;

template<typename T, std::size_t tInlineElements>
using fixed_indirect_array = indirect_array<T, FixedAllocatePolicy<tInlineElements> >;
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
template<typename T, typename Allocator>
class container_trait<indirect_array<T, Allocator>> : public container_trait_base<indirect_array<T, Allocator>>
{
public:
	static const bool MoveWillEmptyContainer = container_trait<typename indirect_array<T, Allocator>::storage_type>::MoveWillEmptyContainer;
};
//////////////////////////////////////////////////////////////////////////

NS_END

//////////////////////////////////////////////////////////////////////////
template <typename T, typename Allocator>
void* operator new(std::size_t inSize, bzbee::indirect_array<T, Allocator>& inArray)
{
	ASSERT(inSize == sizeof(T));
	const std::size_t index = inArray.uninitializedBack((T*)std::malloc(inSize));
	return inArray.data()[index];
}

template <typename T, typename Allocator>
void operator delete(void*, bzbee::indirect_array<T, Allocator>&)
{
	ASSERT_UNREACHABLE;
}
//////////////////////////////////////////////////////////////////////////
