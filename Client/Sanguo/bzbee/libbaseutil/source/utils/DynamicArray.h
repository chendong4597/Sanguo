/*!
 * \file SimpleArray.h
 *
 * \author mattho
 * \date 2016/10/12
 *
 *
 */
#pragma once

#include "ContainerMemory.h"
#include "Sorting.h"
#include <initializer_list>
#include <type_traits>

NS_BEGIN

//////////////////////////////////////////////////////////////////////////
template<typename FromArrayType, typename ToArrayType>
class can_move_array_pointers_between_array_types
{
	typedef typename FromArrayType::allocator_type	FromAllocatorType;
	typedef typename ToArrayType::allocator_type	ToAllocatorType;
	typedef typename FromArrayType::value_type		FromValueType;
	typedef typename ToArrayType::value_type		ToValueType;

public:
	static const bool value =
		// allocators must be equal
		std::is_same<ToAllocatorType, FromAllocatorType>::value &&
		// a move must be allowed to leave the source array empty
		container_trait<FromArrayType>::MoveWillEmptyContainer &&
		// the element type of the container must be the same, or...
		(std::is_same<ToValueType, FromValueType>::value ||
		// ... the element type of the source container must be bitwise constructible from the element type in the destination container
		is_bitwise_constructible<ToValueType, FromValueType>::value);
};
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
/** The allocation policy interface; it exists purely to document the policy's interface, and should not be used. */
class ContainerAllocatorTemplate
{
public:
	/** Determines whether the user of the allocator may use the AnyElementAllocator inner class. */
	static const bool NeedsElementType = true;

	template<typename ElementType>
	class ElementAllocator
	{
	public:
		typedef std::size_t		size_type;

		void moveToEmpty(ElementAllocator& inOther);
		/** Accesses the container's current data. */
		ElementType* getAllocation() const;
		void resizeAllocation(size_type inPrevNumElements, size_type inNumElements, const size_type inNumBytesPerElement);
		size_type calculateSlackReserve(size_type inNumElements, const size_type inNumBytesPerElement) const;
		size_type calculateSlackShrink(size_type inNumElements, size_type inMaxElements, const size_type inNumBytesPerElement) const;
		size_type calculateSlackGrow(size_type inNumElements, size_type inMaxElements, const size_type inNumBytesPerElement) const;
		size_type getAllocatedSize(size_type nMaxElements, const size_type inNumBytesPerElement) const;
	};

	typedef ElementAllocator<detail::ContainerElement> AnyElementAllocator;
};
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
class HeapAllocatePolicy
{
public:
	static const bool NeedsElementType = false;

	class AnyElementAllocator
	{
	public:
		typedef std::size_t		size_type;

		AnyElementAllocator() = default;

		FORCEINLINE ~AnyElementAllocator()
		{
			if (m_data)
			{
				std::free(m_data);
			}
		}

		AnyElementAllocator(const AnyElementAllocator&) = delete;
		AnyElementAllocator& operator=(const AnyElementAllocator&) = delete;

		/** Moves the state of another allocator into this one. */
		FORCEINLINE void moveToEmpty(AnyElementAllocator& inOther)
		{
			ASSERT(this != &inOther);
			if (m_data)
			{
				std::free(m_data);
			}
			m_data = inOther.m_data;
			inOther.m_data = nullptr;
		}

		FORCEINLINE bool hasAllocation() const
		{
			return !!m_data;
		}

		FORCEINLINE detail::ContainerElement* getAllocation() const
		{
			return m_data;
		}

		FORCEINLINE void resizeAllocation(size_type, size_type inNumElements, const size_type inNumBytesPerElement)
		{
			if (m_data || inNumElements)
			{
				m_data = (detail::ContainerElement*)std::realloc(m_data, inNumElements * inNumBytesPerElement);
			}
		}

		FORCEINLINE size_type calculateSlackReserve(size_type inNumElements, const size_type) const
		{
			return inNumElements;
		}

		FORCEINLINE size_type calculateSlackShrink(size_type inNumElements, size_type inMaxElements, const size_type inNumBytesPerElement) const
		{
			return detail::defaultCalculateSlackShrink(inNumElements, inMaxElements, inNumBytesPerElement);
		}

		FORCEINLINE size_type calculateSlackGrow(size_type inNumElements, size_type inMaxElements, const size_type inNumBytesPerElement) const
		{
			return detail::defaultCalculateSlackGrow(inNumElements, inMaxElements, inNumBytesPerElement);
		}

		FORCEINLINE size_type getAllocatedSize(size_type nMaxElements, const size_type inNumBytesPerElement) const
		{
			return nMaxElements * inNumBytesPerElement;
		}

	protected:
		detail::ContainerElement*	m_data{};
	};

	template<typename ElementType>
	class ElementAllocator : public AnyElementAllocator
	{
	public:
		FORCEINLINE ElementType* getAllocation() const
		{
			return (ElementType*)AnyElementAllocator::getAllocation();
		}
	};
};
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
template<std::size_t tInlineElements, typename SecondaryPolicy = HeapAllocatePolicy>
class InlineAllocatePolicy
{
public:
	static const bool NeedsElementType = true;

	typedef void AnyElementAllocator;

	template<typename ElementType>
	class ElementAllocator
	{
	public:
		typedef std::size_t		size_type;

		ElementAllocator() = default;

		ElementAllocator(const ElementAllocator&) = delete;
		ElementAllocator& operator=(const ElementAllocator&) = delete;

		/** Moves the state of another allocator into this one. */
		FORCEINLINE void moveToEmpty(ElementAllocator& inOther)
		{
			ASSERT(this != &inOther);
			
			if (!inOther.m_secondaryData.getAllocation())
			{
				detail::relocateConstructElements<ElementType>((void*)m_inlineData, inOther.getInlineElements(), tInlineElements);
			}

			// Move secondary storage in any case.
			// This will move secondary storage if it exists but will also handle the case where secondary storage is used in Other but not in *this.
			m_secondaryData.moveToEmpty(inOther.m_secondaryData);
		}

		FORCEINLINE void resizeAllocation(size_type inPrevNumElements, size_type inNumElements, const size_type inNumBytesPerElement)
		{
			if (inNumElements <= tInlineElements)
			{
				if (m_secondaryData.getAllocation())
				{
					detail::relocateConstructElements<ElementType>((void*)m_inlineData, m_secondaryData.getAllocation(), inPrevNumElements);
					m_secondaryData.resizeAllocation(0, 0, inNumBytesPerElement);
				}
			}
			else
			{
				if (!m_secondaryData.getAllocation())
				{
					m_secondaryData.resizeAllocation(0, inNumElements, inNumBytesPerElement);
					detail::relocateConstructElements<ElementType>((void*)m_secondaryData.getAllocation(), getInlineElements(), inPrevNumElements);
				}
				else
				{
					m_secondaryData.resizeAllocation(inPrevNumElements, inNumElements, inNumBytesPerElement);
				}
			}
		}

		FORCEINLINE size_type calculateSlackReserve(size_type inNumElements, const size_type inNumBytesPerElement) const
		{
			return (inNumElements <= tInlineElements)
				? tInlineElements
				: m_secondaryData.calculateSlackReserve(inNumElements, inNumBytesPerElement);
		}

		FORCEINLINE size_type calculateSlackShrink(size_type inNumElements, size_type inMaxElements, const size_type inNumBytesPerElement) const
		{
			return (inNumElements <= tInlineElements)
				? tInlineElements
				: m_secondaryData.calculateSlackShrink(inNumElements, inMaxElements, inNumBytesPerElement);
		}

		FORCEINLINE size_type calculateSlackGrow(size_type inNumElements, size_type inMaxElements, const size_type inNumBytesPerElement) const
		{
			return (inNumElements <= tInlineElements)
				? tInlineElements
				: m_secondaryData.calculateSlackGrow(inNumElements, inMaxElements, inNumBytesPerElement);
		}

		FORCEINLINE size_type getAllocatedSize(size_type nMaxElements, const size_type inNumBytesPerElement) const
		{
			return m_secondaryData.getAllocatedSize(nMaxElements, inNumBytesPerElement);
		}

		FORCEINLINE bool hasAllocation() const
		{
			return m_secondaryData.hasAllocation();
		}

		FORCEINLINE ElementType* getAllocation() const
		{
			return returnIfAElseB<ElementType>(m_secondaryData.getAllocation(), getInlineElements());
		}

	private:
		FORCEINLINE ElementType* getInlineElements() const { return (ElementType*)m_inlineData; }

		/** Chooses between the two parameters based on whether the first is nullptr or not. */
		template<typename ReferencedType>
		FORCEINLINE ReferencedType* returnIfAElseB(ReferencedType* inA, ReferencedType* inB) const
		{
			const std::intptr_t nPtrA = reinterpret_cast<std::intptr_t>(inA);
			const std::intptr_t nPtrB = reinterpret_cast<std::intptr_t>(inB);

			// Compute a mask which has all bits set if nPtrA is zero, and no bits set if it's non-zero.
			const std::intptr_t nMaskB = -(!nPtrA);
			return reinterpret_cast<ReferencedType*>(nPtrA | (nMaskB & nPtrB));
		}

	protected:
		type_compatible_bytes<ElementType> m_inlineData[tInlineElements];
		typename SecondaryPolicy::template ElementAllocator<ElementType>	m_secondaryData;
	};
};
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
template<std::size_t tInlineElements>
class FixedAllocatePolicy
{
public:
	static const bool NeedsElementType = true;

	typedef void AnyElementAllocator;

	template<typename ElementType>
	class ElementAllocator
	{
	public:
		typedef std::size_t		size_type;

		ElementAllocator() = default;

		ElementAllocator(const ElementAllocator&) = delete;
		ElementAllocator& operator=(const ElementAllocator&) = delete;

		/** Moves the state of another allocator into this one. */
		FORCEINLINE void moveToEmpty(ElementAllocator& inOther)
		{
			ASSERT(this != &inOther);
			detail::relocateConstructElements<ElementType>((void*)m_inlineData, inOther.getAllocation(), tInlineElements);
		}

		FORCEINLINE void resizeAllocation(size_type, size_type inNumElements, const size_type inNumBytesPerElement)
		{
			ASSERT(inNumElements <= tInlineElements);
		}

		FORCEINLINE size_type calculateSlackReserve(size_type inNumElements, const size_type) const
		{
			ASSERT(inNumElements <= tInlineElements);
			return tInlineElements;
		}

		FORCEINLINE size_type calculateSlackShrink(size_type inNumElements, size_type inMaxElements, const size_type inNumBytesPerElement) const
		{
			ASSERT(inNumElements <= tInlineElements);
			return tInlineElements;
		}

		FORCEINLINE size_type calculateSlackGrow(size_type inNumElements, size_type inMaxElements, const size_type inNumBytesPerElement) const
		{
			ASSERT(inNumElements <= tInlineElements);
			return tInlineElements;
		}

		FORCEINLINE size_type getAllocatedSize(size_type nMaxElements, const size_type inNumBytesPerElement) const
		{
			return 0;
		}

		FORCEINLINE bool hasAllocation() const
		{
			return false;
		}

		FORCEINLINE ElementType* getAllocation() const
		{
			return getInlineElements();
		}

	private:
		FORCEINLINE ElementType* getInlineElements() const { return (ElementType*)m_inlineData; }

	protected:
		type_compatible_bytes<ElementType> m_inlineData[tInlineElements];
	};
};
//////////////////////////////////////////////////////////////////////////

template<typename T, typename Allocator = HeapAllocatePolicy>
class dynamic_array;

//////////////////////////////////////////////////////////////////////////
template<>
class allocation_trait<HeapAllocatePolicy> : public allocation_trait_base<HeapAllocatePolicy>
{
public:
	static const bool SupportsMove = true;
	static const bool IsZeroConstruct = true;
};

template<uint32 tInlineElements, typename SecondaryPolicy>
class allocation_trait<InlineAllocatePolicy<tInlineElements, SecondaryPolicy>> : public allocation_trait_base<InlineAllocatePolicy<tInlineElements, SecondaryPolicy>>
{
public:
	static const bool SupportsMove = allocation_trait<SecondaryPolicy>::SupportsMove;
};

template<uint32 tInlineElements>
class allocation_trait<FixedAllocatePolicy<tInlineElements>> : public allocation_trait_base<FixedAllocatePolicy<tInlineElements>>
{
public:
	static const bool SupportsMove = true;
};
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
template<typename ElementType, typename AllocatorType>
class container_trait<dynamic_array<ElementType, AllocatorType>> : public container_trait_base<dynamic_array<ElementType, AllocatorType>>
{
public:
	static const bool MoveWillEmptyContainer = allocation_trait<AllocatorType>::SupportsMove;
};
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
namespace detail
{
	template<typename ContainerType, typename ElementType, typename IndexType>
	class IndexedContainerIterator
	{
	public:
		typedef std::ptrdiff_t	difference_type;
		typedef ElementType		value_type;
		typedef ElementType*	pointer;
		typedef ElementType&	reference;
		typedef std::random_access_iterator_tag iterator_category;

		IndexedContainerIterator(ContainerType& inContainer, IndexType nStartIndex = 0)
			: m_container(inContainer)
			, m_index(nStartIndex)
		{}

		IndexedContainerIterator(const IndexedContainerIterator& inOther)
			: m_container(inOther.m_container)
			, m_index(inOther.m_index)
		{}

		FORCEINLINE IndexedContainerIterator& operator=(const IndexedContainerIterator& inOther)
		{
			m_container = inOther.m_container;
			m_index = inOther.m_index;
			return *this;
		}

		FORCEINLINE IndexedContainerIterator& operator++()
		{
			++m_index;
			return *this;
		}
		FORCEINLINE IndexedContainerIterator operator++(int)
		{
			IndexedContainerIterator inPrev(*this);
			++m_index;
			return inPrev;
		}

		FORCEINLINE IndexedContainerIterator& operator--()
		{
			--m_index;
			return *this;
		}
		FORCEINLINE IndexedContainerIterator operator--(int)
		{
			IndexedContainerIterator inNext(*this);
			--m_index;
			return inNext;
		}

		FORCEINLINE IndexedContainerIterator& operator+=(difference_type nOffset)
		{
			m_index += nOffset;
			return *this;
		}
		FORCEINLINE IndexedContainerIterator operator+(difference_type nOffset) const
		{
			IndexedContainerIterator inCur(*this);
			return inCur += nOffset;
		}

		FORCEINLINE IndexedContainerIterator& operator-=(difference_type nOffset)
		{
			return *this += -nOffset;
		}
		FORCEINLINE IndexedContainerIterator operator-(difference_type nOffset) const
		{
			IndexedContainerIterator inCur(*this);
			return inCur -= nOffset;
		}

		FORCEINLINE difference_type operator-(const IndexedContainerIterator& inOther) const
		{
			ASSERT(&m_container == &inOther.m_container);
			return getElementDataNoCheck() - inOther.getElementDataNoCheck();
		}

		FORCEINLINE ElementType& operator*() const
		{
			return m_container[m_index];
		}
		FORCEINLINE ElementType* operator->() const
		{
			return &m_container[m_index];
		}

		FORCEINLINE explicit operator bool() const
		{
			return isDereferenceable();
		}
		FORCEINLINE bool operator!() const
		{
			return !(bool)*this;
		}

		FORCEINLINE bool isValid() const
		{
			return (m_index >= 0 && m_index <= m_container.size());
		}

		FORCEINLINE bool isDereferenceable() const
		{
			return (m_index >= 0 && m_index < m_container.size());
		}

		FORCEINLINE IndexType getIndex() const
		{
			return m_index;
		}

		FORCEINLINE void reset()
		{
			m_index = 0;
		}

		FORCEINLINE friend bool operator==(const IndexedContainerIterator& lhs, const IndexedContainerIterator& rhs)
		{
			return (&lhs.m_container == &rhs.m_container) && (lhs.m_index == rhs.m_index);
		}
		FORCEINLINE friend bool operator!=(const IndexedContainerIterator& lhs, const IndexedContainerIterator& rhs)
		{
			return (&lhs.m_container != &rhs.m_container) || (lhs.m_index != rhs.m_index);
		}

		FORCEINLINE friend bool operator<(const IndexedContainerIterator& inA, const IndexedContainerIterator& inB)
		{
			return inA.getElementDataNoCheck() < inB.getElementDataNoCheck();
		}
		FORCEINLINE friend bool operator>(const IndexedContainerIterator& inA, const IndexedContainerIterator& inB)
		{
			return (inB < inA);
		}
		FORCEINLINE friend bool operator>=(const IndexedContainerIterator& inA, const IndexedContainerIterator& inB)
		{
			return !(inA < inB);
		}
		FORCEINLINE friend bool operator<=(const IndexedContainerIterator& inA, const IndexedContainerIterator& inB)
		{
			return !(inA > inB);
		}

	private:
		FORCEINLINE ElementType* getElementDataNoCheck() const
		{
			ASSERT(isValid() == true);
			return m_container.data() + m_index;
		}

	protected:
		ContainerType&		m_container;
		IndexType			m_index;
	};

	template<typename ContainerType, typename ElementType, typename IndexType>
	FORCEINLINE IndexedContainerIterator<ContainerType, ElementType, IndexType> operator+(
		typename IndexedContainerIterator<ContainerType, ElementType, IndexType>::difference_type nOffset, IndexedContainerIterator<ContainerType, ElementType, IndexType> inRhs)
	{
		return inRhs + nOffset;
	}
}
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
template<typename T, typename Allocator>
class dynamic_array
{
public:
	typedef typename std::conditional<Allocator::NeedsElementType,
		typename Allocator::template ElementAllocator<T>,
		typename Allocator::AnyElementAllocator>::type	allocator_type;
	
	typedef	T			value_type;
	typedef typename allocator_type::size_type	size_type;
	typedef std::ptrdiff_t	difference_type;
	typedef T*			pointer;
	typedef const T*	const_pointer;
	typedef T&			reference;
	typedef const T&	const_reference;

	typedef detail::IndexedContainerIterator<dynamic_array, value_type, size_type>				iterator;
	typedef detail::IndexedContainerIterator<const dynamic_array, const value_type, size_type>	const_iterator;

public:
	dynamic_array() = default;

	dynamic_array(std::initializer_list<T> inInitList)
	{
		copyToEmpty(inInitList.begin(), inInitList.size(), 0, 0);
	}

	FORCEINLINE dynamic_array(const dynamic_array& inOther)
	{
		copyToEmpty(inOther.data(), inOther.size(), 0, 0);
	}

	FORCEINLINE dynamic_array(const dynamic_array& inOther, size_type inExtraSlack)
	{
		copyToEmpty(inOther.data(), inOther.size(), 0, inExtraSlack);
	}

	template<typename OtherElementType, typename OtherAllocator>
	FORCEINLINE explicit dynamic_array(const dynamic_array<OtherElementType, OtherAllocator>& inOther)
	{
		copyToEmpty(inOther.data(), inOther.size(), 0, 0);
	}

	FORCEINLINE dynamic_array(dynamic_array&& inOther)
	{
		moveOrCopy(*this, inOther, 0);
	}

	template<typename OtherElementType, typename OtherAllocator>
	FORCEINLINE explicit dynamic_array(dynamic_array<OtherElementType, OtherAllocator>&& inOther)
	{
		moveOrCopy(*this, inOther, 0);
	}

	template<typename OtherElementType>
	FORCEINLINE dynamic_array(dynamic_array<OtherElementType, Allocator>&& inOther, size_type inExtraSlack)
	{
		moveOrCopyWithSlack(*this, inOther, 0, inExtraSlack);
	}

	FORCEINLINE explicit dynamic_array(size_type inInitialSlack)
	{
		reserveTo(inInitialSlack);
	}

	~dynamic_array()
	{
		detail::destructElements(data(), m_arrayNum);
	}

	dynamic_array& operator=(std::initializer_list<T> inInitList)
	{
		detail::destructElements(data(), m_arrayNum);
		copyToEmpty(inInitList.begin(), inInitList.size(), m_arrayMax, 0);
		return *this;
	}

	dynamic_array& operator=(const dynamic_array& inOther)
	{
		if (this != &inOther)
		{
			detail::destructElements(data(), m_arrayNum);
			copyToEmpty(inOther.data(), inOther.size(), m_arrayMax, 0);
		}
		return *this;
	}

	template<typename OtherAllocator>
	dynamic_array& operator=(const dynamic_array<T, OtherAllocator>& inOther)
	{
		detail::destructElements(data(), m_arrayNum);
		copyToEmpty(inOther.data(), inOther.size(), m_arrayMax, 0);
		return *this;
	}

	dynamic_array& operator=(dynamic_array&& inOther)
	{
		if (this != &inOther)
		{
			detail::destructElements(data(), m_arrayNum);
			moveOrCopy(*this, inOther, m_arrayMax);
		}
		return *this;
	}

	FORCEINLINE dynamic_array& operator+=(const dynamic_array& inOther)
	{
		append(inOther);
		return *this;
	}

	FORCEINLINE dynamic_array& operator+=(dynamic_array&& inOther)
	{
		append(std::move(inOther));
		return *this;
	}

	FORCEINLINE dynamic_array& operator+=(std::initializer_list<T> inInitList)
	{
		append(inInitList.begin(), inInitList.size());
		return *this;
	}

	template<typename OtherElementType, typename OtherAllocator>
	void append(const dynamic_array<OtherElementType, OtherAllocator>& inOther)
	{
		ASSERT((void*)this != (void*)&inOther);

		const size_type nAppendElements = inOther.size();
		if (!nAppendElements) return;
		
		reserve(m_arrayNum + nAppendElements);
		detail::constructElements<value_type>(data() + m_arrayNum, inOther.data(), nAppendElements);

		m_arrayNum += nAppendElements;
	}

	template<typename OtherElementType, typename OtherAllocator>
	void append(dynamic_array<OtherElementType, OtherAllocator>&& inOther)
	{
		ASSERT((void*)this != (void*)&inOther);

		const size_type nAppendElements = inOther.size();
		if (!nAppendElements) return;

		reserve(m_arrayNum + nAppendElements);
		detail::relocateConstructElements<value_type>(data() + m_arrayNum, inOther.data(), nAppendElements);

		inOther.m_arrayNum = 0;
		m_arrayNum += nAppendElements;
	}

	void append(const value_type* inOtherElements, const size_type inNumAppendElements)
	{
		ASSERT(inOtherElements != nullptr);

		const size_type iAppendIndex = uninitializedBack(inNumAppendElements);
		detail::constructElements<value_type>(data() + iAppendIndex, inOtherElements, inNumAppendElements);
	}

	FORCEINLINE value_type pop_back(bool bAllowShrinking = false)
	{
		ASSERT((m_arrayNum >= 1) & (m_arrayNum <= m_arrayMax));
		value_type retval = std::move(data()[m_arrayNum - 1]);
		removeAt(m_arrayNum - 1, 1, bAllowShrinking);
		return retval;
	}

	FORCEINLINE value_type& last(const size_type inOffsetLeft = 0)
	{
		ASSERT((m_arrayNum - inOffsetLeft - 1) < m_arrayNum);
		return data()[m_arrayNum - inOffsetLeft - 1];
	}

	FORCEINLINE const value_type& last(const size_type inOffsetLeft = 0) const
	{
		ASSERT((m_arrayNum - inOffsetLeft - 1) < m_arrayNum);
		return data()[m_arrayNum - inOffsetLeft - 1];
	}

	FORCEINLINE friend bool operator==(const dynamic_array<T, Allocator>& lhs, const dynamic_array<T, Allocator>& rhs)
	{
		const size_type nNumElements = lhs.size();
		return (nNumElements == rhs.size() && detail::isEqualElements(lhs.data(), rhs.data(), nNumElements));
	}

	FORCEINLINE friend bool operator!=(const dynamic_array<T, Allocator>& lhs, const dynamic_array<T, Allocator>& rhs)
	{
		return !(lhs == rhs);
	}

	//// Element access
	FORCEINLINE reference operator[](size_type index)
	{
		ASSERT((index >= 0) & (index < m_arrayNum));
		return data()[index];
	}

	FORCEINLINE const_reference operator[](size_type index) const
	{
		ASSERT((index >= 0) & (index < m_arrayNum));
		return data()[index];
	}

	FORCEINLINE pointer data() { return (pointer)m_allocator.getAllocation(); }
	FORCEINLINE const_pointer data() const { return (const_pointer)m_allocator.getAllocation(); }

	//// Iterators
	FORCEINLINE iterator begin() { return iterator(*this); }
	FORCEINLINE iterator end() { return iterator(*this, m_arrayNum); }
	FORCEINLINE const_iterator begin() const { return const_iterator(*this); }
	FORCEINLINE const_iterator end() const { return const_iterator(*this, m_arrayNum); }

	//// Capacity
	FORCEINLINE bool empty() const { return (m_arrayNum == 0); }
	FORCEINLINE size_type size() const { return m_arrayNum; }
	FORCEINLINE void reserve(size_type inNewCapacity) { if (inNewCapacity > m_arrayMax) reserveTo(inNewCapacity); }
	FORCEINLINE size_type capacity() const { return m_arrayMax; }
	FORCEINLINE void shrink_to_fit() { if (m_arrayMax != m_arrayNum) reserveTo(m_arrayNum); }

	FORCEINLINE size_type sizeInBytes() const { return (m_arrayNum * sizeof(value_type)); }

	//// Modifiers
	FORCEINLINE size_type push_back(value_type&& inElement)
	{
		ASSERT(&inElement < data() || &inElement >= (data() + m_arrayMax));
		return emplace_back(std::move(inElement));
	}

	FORCEINLINE size_type push_back(const value_type& inElement)
	{
		ASSERT(&inElement < data() || &inElement >= (data() + m_arrayMax));
		return emplace_back(inElement);
	}

	template<typename... ParamTypes>
	FORCEINLINE size_type emplace_back(ParamTypes&&... inArgs)
	{
		const size_type nIndex = uninitializedBack(1);
		new(data() + nIndex) value_type(std::forward<ParamTypes>(inArgs)...);
		return nIndex;
	}

	/** Adds new items to the end of the array. The new items will be zeroed. */
	FORCEINLINE size_type zeroedBack(size_type inAddElements = 1)
	{
		const size_type nIndex = uninitializedBack(inAddElements);
		std::memset((uint8*)m_allocator.getAllocation() + nIndex * sizeof(value_type), 0, inAddElements * sizeof(value_type));
		return nIndex;
	}

	/** Adds a given number of uninitialized elements into the array. */
	FORCEINLINE size_type uninitializedBack(size_type inAddElements = 1)
	{
		ASSERT((m_arrayNum >= 0) & (m_arrayNum <= m_arrayMax));
		ASSERT(inAddElements >= 0);

		const size_type inPrevNumElements = m_arrayNum;
		if ((m_arrayNum += inAddElements) > m_arrayMax)
		{
			resizeGrow(inPrevNumElements);
		}
		return inPrevNumElements;
	}

	/**
	 * Adds unique element to array if it doesn't exist.
	 *
	 * \return	A pair consisting of an index to the added element (or to the element that prevented the insertion)
	 *			and a bool denoting whether the insertion took place.
	 */
	FORCEINLINE std::pair<size_type, bool> uniqueBack(value_type&& inElement)
	{
		return uniqueBackImpl(std::move(inElement));
	}

	/**
	 * Adds unique element to array if it doesn't exist.
	 *
	 * \return	A pair consisting of an index to the added element (or to the element that prevented the insertion)
	 *			and a bool denoting whether the insertion took place.
	 */
	FORCEINLINE std::pair<size_type, bool> uniqueBack(const value_type& inElement)
	{
		return uniqueBackImpl(inElement);
	}

	/** Adds a given number of uninitialized elements into the array. */
	FORCEINLINE size_type uninitializedInsert(size_type inIndex, size_type inAddElements = 1)
	{
		ASSERT((m_arrayNum >= 0) & (m_arrayNum <= m_arrayMax));
		ASSERT(inAddElements >= 0);

		const size_type inPrevNumElements = m_arrayNum;
		if ((m_arrayNum += inAddElements) > m_arrayMax)
		{
			resizeGrow(inPrevNumElements);
		}
		detail::shiftRight(m_allocator.getAllocation(), inPrevNumElements, inIndex, inAddElements, sizeof(value_type));
		return inIndex;
	}

	template<typename... ParamTypes>
	FORCEINLINE size_type emplace(size_type inIndex, ParamTypes&&... inArgs)
	{
		uninitializedInsert(inIndex, 1);
		new(data() + inIndex) value_type(std::forward<ParamTypes>(inArgs)...);
		return inIndex;
	}

	FORCEINLINE iterator erase(iterator inPosition)
	{
		ASSERT(!!inPosition);
		return iterator(*this, removeAt(inPosition.getIndex(), 1, false));
	}

	FORCEINLINE const_iterator erase(const_iterator inPosition)
	{
		ASSERT(!!inPosition);
		return const_iterator(*this, removeAt(inPosition.getIndex(), 1, false));
	}

	size_type removeAt(size_type inIndex, size_type inNumElements = 1, bool bAllowShrinking = false)
	{
		if (!inNumElements) return npos;

		ASSERT((m_arrayNum >= 0) & (m_arrayNum <= m_arrayMax));
		ASSERT((inNumElements >= 0) & (inIndex >= 0) & (inIndex + inNumElements <= m_arrayNum));

		detail::destructElements(data() + inIndex, inNumElements);

		detail::shiftLeft(m_allocator.getAllocation(), m_arrayNum, inIndex, inNumElements, sizeof(value_type));
		m_arrayNum -= inNumElements;

		if (bAllowShrinking) reserveShrink();
		return inIndex;
	}

	void removeAtSwap(size_type inIndex, size_type inNumElements = 1, bool bAllowShrinking = false)
	{
		if (!inNumElements) return;

		ASSERT((m_arrayNum >= 0) & (m_arrayNum <= m_arrayMax));
		ASSERT((inNumElements >= 0) & (inIndex >= 0) & (inIndex + inNumElements <= m_arrayNum));

		detail::destructElements(data() + inIndex, inNumElements);

		const size_type nNumElementsInHole = inNumElements;
		const size_type nNumElementsAfterHole = m_arrayNum - (inIndex + inNumElements);
		const size_type nNumElementsToMoveIntoHole = std::min(nNumElementsInHole, nNumElementsAfterHole);
		if (nNumElementsToMoveIntoHole)
		{
			std::memcpy(
				(uint8*)m_allocator.getAllocation() + (inIndex)* sizeof(value_type),
				(uint8*)m_allocator.getAllocation() + (m_arrayNum - nNumElementsToMoveIntoHole) * sizeof(value_type),
				(nNumElementsToMoveIntoHole)* sizeof(value_type));
		}
		m_arrayNum -= inNumElements;

		if (bAllowShrinking) reserveShrink();
	}

	FORCEINLINE bool removeSingle(const value_type& inElement)
	{
		const size_type nFoundIndex = findIndexOf(inElement);
		if (nFoundIndex == npos) return false;

		removeAt(nFoundIndex, 1, false);
		return true;
	}

	FORCEINLINE bool removeSingleSwap(const value_type& inElement)
	{
		const size_type nFoundIndex = findIndexOf(inElement);
		if (nFoundIndex == npos) return false;

		removeAtSwap(nFoundIndex, 1, false);
		return true;
	}

	FORCEINLINE void clear()
	{
		detail::destructElements(data(), m_arrayNum);
		m_arrayNum = 0;
	}

	FORCEINLINE void purge(size_type inSlack = 0)
	{
		clear();
		if (m_arrayMax != inSlack)
		{
			reserveTo(inSlack);
		}
	}

	void resize(size_type inNewElements, bool bAllowShrinking = false)
	{
		if (inNewElements > m_arrayNum)
		{
			const size_type nDiffElements = inNewElements - m_arrayNum;
			const size_type nIndex = uninitializedBack(nDiffElements);
			detail::defaultConstructElements<value_type>((uint8*)m_allocator.getAllocation() + nIndex * sizeof(value_type), nDiffElements);
		}
		else if (inNewElements < m_arrayNum)
		{
			removeAt(inNewElements, m_arrayNum - inNewElements, bAllowShrinking);
		}
	}

	static const size_type npos = (size_type)-1;

	//// Misc
	/** Find the array element assuming == operator is defined for the item type.*/
	FORCEINLINE std::pair<size_type, bool> find(const value_type& inElement) const
	{
		const size_type nFoundIndex = this->findIndexOf(inElement);
		return std::pair<size_type, bool>(nFoundIndex, nFoundIndex != npos);
	}

	template<typename PredicateType>
	std::pair<size_type, bool> findByPredicate(PredicateType inPredicate) const
	{
		const size_type nFoundIndex = this->findIndexOfByPredicate(inPredicate);
		return std::pair<size_type, bool>(nFoundIndex, nFoundIndex != npos);
	}

	size_type findIndexOf(const value_type& inElement) const
	{
		const value_type* __restrict BeginIter = data();
		for (const value_type* __restrict Iter = BeginIter, *__restrict EndIter = Iter + m_arrayNum; Iter != EndIter; ++Iter)
		{
			if (*Iter == inElement)
			{
				return static_cast<size_type>(Iter - BeginIter);
			}
		}
		return npos;
	}

	template<typename PredicateType>
	size_type findIndexOfByPredicate(PredicateType inPredicate) const
	{
		const value_type* __restrict BeginIter = data();
		for (const value_type* __restrict Iter = BeginIter, *__restrict EndIter = Iter + m_arrayNum; Iter != EndIter; ++Iter)
		{
			if (inPredicate(*Iter))
			{
				return static_cast<size_type>(Iter - BeginIter);
			}
		}
		return npos;
	}

	/** Sorts the array assuming < operator is defined for the item type.*/
	FORCEINLINE void sort()
	{
		detail::sort(data(), m_arrayNum);
	}

	template<typename PredicateType>
	FORCEINLINE void sort(const PredicateType& inPredicate)
	{
		detail::sort(data(), m_arrayNum, inPredicate);
	}

protected:
	template<typename ParamType>
	std::pair<size_type, bool> uniqueBackImpl(ParamType&& inArg)
	{
		const auto rFound = find(inArg);
		const size_type nIndex = rFound.second
			? rFound.first
			: emplace_back(std::forward<ParamType>(inArg));
		return std::pair<size_type, bool>(nIndex, !rFound.second);
	}

	void resizeGrow(const size_type inPrevNumElements)
	{
		m_arrayMax = m_allocator.calculateSlackGrow(m_arrayNum, m_arrayMax, sizeof(value_type));
		m_allocator.resizeAllocation(inPrevNumElements, m_arrayMax, sizeof(value_type));
	}

	void reserveShrink()
	{
		const size_type nNewArrayMax = m_allocator.calculateSlackShrink(m_arrayNum, m_arrayMax, sizeof(value_type));
		if (nNewArrayMax != m_arrayMax)
		{
			m_arrayMax = nNewArrayMax;
			ASSERT(m_arrayMax >= m_arrayNum);
			m_allocator.resizeAllocation(m_arrayNum, m_arrayMax, sizeof(value_type));
		}
	}

	void reserveTo(size_type inMaxElements)
	{
		if (inMaxElements)
		{
			inMaxElements = m_allocator.calculateSlackReserve(inMaxElements, sizeof(value_type));
		}
		if (inMaxElements != m_arrayMax)
		{
			m_arrayMax = inMaxElements;
			m_allocator.resizeAllocation(m_arrayNum, m_arrayMax, sizeof(value_type));
		}
	}

	void reserveForCopy(size_type inMaxElements, const size_type inPrevMaxElements)
	{
		if (inMaxElements)
		{
			inMaxElements = m_allocator.calculateSlackReserve(inMaxElements, sizeof(value_type));
		}
		if (inMaxElements != inPrevMaxElements)
		{
			m_allocator.resizeAllocation(0, inMaxElements, sizeof(value_type));
		}
		m_arrayMax = inMaxElements;
	}

	template<typename OtherElementType>
	void copyToEmpty(const OtherElementType* inSourceElements, const size_type inNumSourceElements, size_type inPrevMaxElements, size_type inExtraSlack)
	{
		ASSERT(inExtraSlack >= 0);

		m_arrayNum = inNumSourceElements;
		if (inNumSourceElements || inExtraSlack || inPrevMaxElements)
		{
			reserveForCopy(inNumSourceElements + inExtraSlack, inPrevMaxElements);
			detail::constructElements<value_type>(data(), inSourceElements, inNumSourceElements);
		}
		else
		{
			m_arrayMax = 0;
		}
	}

	template<typename FromArrayType, typename ToArrayType>
	FORCEINLINE static typename std::enable_if<can_move_array_pointers_between_array_types<FromArrayType, ToArrayType>::value>::type moveOrCopy(ToArrayType& inDestArray, FromArrayType& inSrcArray, size_type inPrevMaxElements)
	{
		inDestArray.m_allocator.moveToEmpty(inSrcArray.m_allocator);

		inDestArray.m_arrayNum = inSrcArray.m_arrayNum;
		inSrcArray.m_arrayNum = 0;

		inDestArray.m_arrayMax = inSrcArray.m_arrayMax;
		inSrcArray.m_arrayMax = 0;
	}

	template<typename FromArrayType, typename ToArrayType>
	FORCEINLINE static typename std::enable_if<!can_move_array_pointers_between_array_types<FromArrayType, ToArrayType>::value>::type moveOrCopy(ToArrayType& inDestArray, FromArrayType& inSrcArray, size_type inPrevMaxElements)
	{
		inDestArray.copyToEmpty(inSrcArray.data(), inSrcArray.size(), inPrevMaxElements, 0);
	}

	template<typename FromArrayType, typename ToArrayType>
	FORCEINLINE static typename std::enable_if<can_move_array_pointers_between_array_types<FromArrayType, ToArrayType>::value>::type moveOrCopyWithSlack(ToArrayType& inDestArray, FromArrayType& inSrcArray, size_type inPrevMaxElements, size_type inExtraSlack)
	{
		moveOrCopy(inDestArray, inSrcArray, inPrevMaxElements);
		inDestArray.reserve(inDestArray.m_arrayNum + inExtraSlack);
	}

	template<typename FromArrayType, typename ToArrayType>
	FORCEINLINE static typename std::enable_if<!can_move_array_pointers_between_array_types<FromArrayType, ToArrayType>::value>::type moveOrCopyWithSlack(ToArrayType& inDestArray, FromArrayType& inSrcArray, size_type inPrevMaxElements, size_type inExtraSlack)
	{
		inDestArray.copyToEmpty(inSrcArray.data(), inSrcArray.size(), inPrevMaxElements, inExtraSlack);
	}

protected:
	allocator_type	m_allocator;

	size_type	m_arrayNum{};
	size_type	m_arrayMax{};
};
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
template<typename T, std::size_t tInlineElements, typename SecondaryPolicy = HeapAllocatePolicy>
using inline_array = dynamic_array<T, InlineAllocatePolicy<tInlineElements, SecondaryPolicy> >;

template<typename T, std::size_t tInlineElements>
using fixed_array = dynamic_array<T, FixedAllocatePolicy<tInlineElements> >;
//////////////////////////////////////////////////////////////////////////

NS_END
