/*!
 * \file IntrusiveList.h
 *
 * \author mattho
 * \date 2016/10/12
 *
 *
 */
#pragma once

#include "../BaseType.h"
#include "../AssertionMacros.h"
#include "ContainerMemory.h"

NS_BEGIN

template<typename T>
class intrusive_list;

//////////////////////////////////////////////////////////////////////////
namespace detail
{
	template<typename ContainerType>
	class intrusive_list_iterator_base
	{
	public:
		explicit intrusive_list_iterator_base(ContainerType* inHeadLink)
			: m_currentLink(inHeadLink)
		{}

		FORCEINLINE void next()
		{
			ASSERT(m_currentLink != nullptr);
			m_currentLink = static_cast<ContainerType*>(m_currentLink->get_next());
		}

		FORCEINLINE intrusive_list_iterator_base& operator++()
		{
			next();
			return *this;
		}

		FORCEINLINE operator bool() const { return m_currentLink != nullptr; }

	protected:
		FORCEINLINE friend bool operator==(const intrusive_list_iterator_base& lhs, const intrusive_list_iterator_base& rhs) { return lhs.m_currentLink == rhs.m_currentLink; }
		FORCEINLINE friend bool operator!=(const intrusive_list_iterator_base& lhs, const intrusive_list_iterator_base& rhs) { return lhs.m_currentLink != rhs.m_currentLink; }

	protected:
		ContainerType*	m_currentLink{};
	};

	template<typename ContainerType, typename ElementType>
	class intrusive_list_iterator : public intrusive_list_iterator_base<ElementType>
	{
		typedef intrusive_list_iterator_base<ElementType>		super_type;

	public:
		explicit intrusive_list_iterator(ElementType* inHeadLink)
			: super_type(inHeadLink)
		{ }

		FORCEINLINE ElementType& operator->() const
		{
			ASSERT(this->m_currentLink != nullptr);
			return *(this->m_currentLink);
		}

		FORCEINLINE ElementType& operator*() const
		{
			ASSERT(this->m_currentLink != nullptr);
			return *(this->m_currentLink);
		}
	};

	template<typename ContainerType, typename ElementType>
	class linked_list_iterator : public intrusive_list_iterator_base<ContainerType>
	{
		typedef intrusive_list_iterator_base<ContainerType>		super_type;

	public:
		explicit linked_list_iterator(ContainerType* inHeadLink)
			: super_type(inHeadLink)
		{ }

		// Accessors.
		FORCEINLINE ElementType& operator->() const
		{
			ASSERT(this->m_currentLink != nullptr);
			return **(this->m_currentLink);
		}

		FORCEINLINE ElementType& operator*() const
		{
			ASSERT(this->m_currentLink != nullptr);
			return **(this->m_currentLink);
		}
	};

	template<typename ContainerType, typename ElementType, template<typename, typename> class IteratorType>
	class list_link
	{
	public:
		list_link() = default;

		FORCEINLINE bool is_linked() const { return m_nextLink != nullptr; }
		FORCEINLINE ContainerType** get_prev() const { return m_prevLink; }
		FORCEINLINE ContainerType* get_next() const { return m_nextLink; }

		FORCEINLINE void unlink()
		{
			if (m_nextLink)
			{
				m_nextLink->m_prevLink = m_prevLink;
			}
			if (m_prevLink)
			{
				*m_prevLink = m_nextLink;
			}

			m_nextLink = nullptr;
			m_prevLink = nullptr;
		}

		/** Insert this element to a list, before the given element. */
		FORCEINLINE void link_before(ContainerType* inPosition)
		{
			ASSERT(inPosition != nullptr);

			m_prevLink = inPosition->m_prevLink;
			inPosition->m_prevLink = &m_nextLink;

			m_nextLink = inPosition;

			if (m_prevLink)
			{
				*m_prevLink = static_cast<ContainerType*>(this);
			}
		}

		FORCEINLINE void link_after(ContainerType* inPosition)
		{
			ASSERT(inPosition != nullptr);

			m_prevLink = &inPosition->m_nextLink;
			m_nextLink = *m_prevLink;

			*m_prevLink = static_cast<ContainerType*>(this);

			if (m_nextLink)
			{
				m_nextLink->m_prevLink = &m_nextLink;
			}
		}

		FORCEINLINE void link_replace(ContainerType* inPosition)
		{
			ASSERT(inPosition != nullptr);

			ContainerType**& replacePrev = inPosition->m_prevLink;
			ContainerType*& replaceNext = inPosition->m_nextLink;

			m_prevLink = replacePrev;
			m_nextLink = replaceNext;

			if (m_prevLink)
			{
				*m_prevLink = static_cast<ContainerType*>(this);
			}
			if (m_nextLink)
			{
				m_nextLink->m_prevLink = &m_nextLink;
			}

			replacePrev = nullptr;
			replaceNext = nullptr;
		}

		FORCEINLINE void link_head(ContainerType*& inHead)
		{
			if (inHead)
			{
				inHead->m_prevLink = &m_nextLink;
			}

			m_nextLink = inHead;
			m_prevLink = &inHead;
			inHead = static_cast<ContainerType*>(this);
		}

	public:
		typedef IteratorType<ContainerType, ElementType>		iterator;
		typedef IteratorType<ContainerType, const ElementType>	const_iterator;

		FORCEINLINE friend iterator begin(ContainerType& inList) { return iterator(&inList); }
		FORCEINLINE friend iterator end(ContainerType& inList) { return iterator(nullptr); }
		FORCEINLINE friend const_iterator begin(const ContainerType& inList) { return const_iterator(const_cast<ContainerType*>(&inList)); }
		FORCEINLINE friend const_iterator end(const ContainerType& inList) { return const_iterator(nullptr); }

	private:
		ContainerType*	m_nextLink{};
		ContainerType**	m_prevLink{};
	};
}
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
template<typename T>
class intrusive_list
	: public detail::list_link<T, T, detail::intrusive_list_iterator>
{
	typedef detail::list_link<T, T, detail::intrusive_list_iterator>	super_type;

public:
	intrusive_list()
		: super_type()
	{}
};
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
template<typename T>
class linked_list
	: public detail::list_link<linked_list<T>, T, detail::linked_list_iterator>
{
	typedef detail::list_link<linked_list<T>, T, detail::linked_list_iterator>	super_type;

public:
	template<typename... ValueType>
	linked_list(ValueType&&... inArgs)
		: super_type()
		, m_value(std::forward<ValueType>(inArgs)...)
	{ }

	FORCEINLINE T& get() { return m_value; }
	FORCEINLINE const T& get() const { return m_value; }

	FORCEINLINE T& operator*() { return m_value; }
	FORCEINLINE const T& operator*() const { return m_value; }

	FORCEINLINE T* operator->() { return &m_value; }
	FORCEINLINE const T* operator->() const { return &m_value; }

private:
	T	m_value;
};
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
/*template<typename T>
class intrusive_linkedlist
{
public:
	typedef intrusive_list<T>	link_type;

	typedef typename link_type::iterator		iterator;
	typedef typename link_type::const_iterator	const_iterator;

	FORCEINLINE bool empty() const
	{
		return (m_head == nullptr);
	}

	FORCEINLINE void emplace(link_type* inNode)
	{
		inNode->link_head(m_head);
	}

	FORCEINLINE link_type* pop()
	{
		link_type* result = m_head;
		if (result) result->unlink();
		return result;
	}

	FORCEINLINE iterator begin() { return iterator(m_head); }
	FORCEINLINE iterator end() { return iterator(nullptr); }
	FORCEINLINE const_iterator begin() const { return const_iterator(m_head); }
	FORCEINLINE const_iterator end() const { return const_iterator(nullptr); }

private:
	link_type*		m_head{};
};*/
//////////////////////////////////////////////////////////////////////////

NS_END
