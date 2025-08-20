/*!
 * \file Optional.h
 *
 * \author mattho
 * \date 2017/04/11
 *
 *
 */
#pragma once

#include "ContainerMemory.h"

NS_BEGIN

//////////////////////////////////////////////////////////////////////////
/**
 * When we have an optional value isValid() returns true, and get() is meaningful.
 * Otherwise get() is not meaningful.
 */
template<typename ElementType>
class Optional
{
public:
	Optional(const ElementType& inValue)
	{
		::new(&m_storage) ElementType(inValue);
		m_bIsValid = true;
	}
	Optional(ElementType&& inValue)
	{
		::new(&m_storage) ElementType(std::move(inValue));
		m_bIsValid = true;
	}

	Optional(const Optional& inValue)
	{
		if (inValue.m_bIsValid)
		{
			::new(&m_storage) ElementType(*(const ElementType*)&inValue.m_storage);
			m_bIsValid = true;
		}
	}
	Optional(Optional&& inValue)
	{
		if (inValue.m_bIsValid)
		{
			::new(&m_storage) ElementType(std::move(*(ElementType*)&inValue.m_storage));
			m_bIsValid = true;
		}
	}

	Optional() = default;
	FORCEINLINE ~Optional()
	{
		reset();
	}

	Optional& operator=(const Optional& inValue)
	{
		if (&inValue != this)
		{
			reset();
			if (inValue.m_bIsValid)
			{
				::new(&m_storage) ElementType(*(const ElementType*)&inValue.m_storage);
				m_bIsValid = true;
			}
		}
		return *this;
	}
	Optional& operator=(Optional&& inValue)
	{
		if (&inValue != this)
		{
			reset();
			if (inValue.m_bIsValid)
			{
				::new(&m_storage) ElementType(std::move(*(ElementType*)&inValue.m_storage));
				m_bIsValid = true;
			}
		}
		return *this;
	}

	Optional& operator=(const ElementType& inValue)
	{
		if (&inValue != (ElementType*)&m_storage)
		{
			reset();
			::new(&m_storage) ElementType(inValue);
			m_bIsValid = true;
		}
		return *this;
	}
	Optional& operator=(ElementType&& inValue)
	{
		if (&inValue != (ElementType*)&m_storage)
		{
			reset();
			::new(&m_storage) ElementType(std::move(inValue));
			m_bIsValid = true;
		}
		return *this;
	}

	template<typename... ParamTypes>
	void emplace(ParamTypes&&... inArgs)
	{
		reset();
		::new(&m_storage) ElementType(std::forward<ParamTypes>(inArgs)...);
		m_bIsValid = true;
	}

	void reset()
	{
		if (m_bIsValid)
		{
			typedef ElementType		OptionalDestructOptionalType;
			((ElementType*)&m_storage)->OptionalDestructOptionalType::~OptionalDestructOptionalType();

			m_bIsValid = false;
		}
	}

	friend bool operator==(const Optional& lhs, const Optional& rhs)
	{
		if (lhs.m_bIsValid != rhs.m_bIsValid)
		{
			return false;
		}
		if (!lhs.m_bIsValid) // both unset
		{
			return true;
		}
		return (*(ElementType*)&lhs.m_storage) == (*(ElementType*)&rhs.m_storage);
	}
	FORCEINLINE friend bool operator!=(const Optional& lhs, const Optional& rhs)
	{
		return !(lhs == rhs);
	}

	/** Return true when the value is meaningful; false if calling get() is undefined. */
	FORCEINLINE bool isValid() const { return m_bIsValid; }
	FORCEINLINE explicit operator bool() const { return m_bIsValid; }

	FORCEINLINE ElementType& get() { ASSERT(isValid()); return *(ElementType*)&m_storage; }
	FORCEINLINE const ElementType& get() const { ASSERT(isValid()); return *(ElementType*)&m_storage; }

	FORCEINLINE const ElementType& getOrDefault(const ElementType& inDefaultValue) const
	{
		return isValid() ? *(ElementType*)&m_storage : inDefaultValue;
	}

	FORCEINLINE ElementType* operator->() { return &get(); }
	FORCEINLINE const ElementType* operator->() const { return &get(); }

	FORCEINLINE operator ElementType&() { return get(); }

private:
	type_compatible_bytes<ElementType>	m_storage;
	bool	m_bIsValid{};
};
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
template<typename ElementType, uint32 tAlignment = std::alignment_of<ElementType>::value>
class OptionalPtr
{
public:
	OptionalPtr(const ElementType& inValue)
	{
		this->emplace(inValue);
	}
	OptionalPtr(ElementType&& inValue)
	{
		this->emplace(std::move(inValue));
	}

	template<uint32 tOther>
	OptionalPtr(const OptionalPtr<ElementType, tOther>& inValue)
	{
		if (inValue.isValid())
		{
			this->emplace(*inValue.m_storage);
		}
	}
	template<uint32 tOther>
	OptionalPtr(OptionalPtr<ElementType, tOther>&& inValue)
	{
		if (inValue.isValid())
		{
			this->emplace(std::move(*inValue.m_storage));
		}
	}

	OptionalPtr() = default;
	FORCEINLINE ~OptionalPtr()
	{
		reset();
	}

	template<uint32 tOther>
	OptionalPtr& operator=(const OptionalPtr<ElementType, tOther>& inValue)
	{
		if (&inValue != this)
		{
			if (inValue.m_storage)
			{
				this->emplace(*inValue.m_storage);
			}
			else
			{
				reset();
			}
		}
		return *this;
	}
	template<uint32 tOther>
	OptionalPtr& operator=(OptionalPtr<ElementType, tOther>&& inValue)
	{
		if (&inValue != this)
		{
			if (inValue.m_storage)
			{
				this->emplace(std::move(*inValue.m_storage));
			}
			else
			{
				reset();
			}
		}
		return *this;
	}

	OptionalPtr& operator=(const ElementType& inValue)
	{
		if (&inValue != (ElementType*)&m_storage)
		{
			this->emplace(inValue);	
		}
		return *this;
	}
	OptionalPtr& operator=(ElementType&& inValue)
	{
		if (&inValue != (ElementType*)&m_storage)
		{
			this->emplace(std::move(inValue));	
		}
		return *this;
	}

	template<typename... ParamTypes>
	void emplace(ParamTypes&&... inArgs)
	{
		if (m_storage)
		{
			destructStorage();
		}
		else
		{
			const uint32 AlignBytes = bzbee::align(sizeof(ElementType), tAlignment);
			m_storage = (ElementType*)std::malloc(AlignBytes);
		}
		::new(m_storage) ElementType(std::forward<ParamTypes>(inArgs)...);
	}

	void reset()
	{
		if (m_storage)
		{
			destructStorage();
			std::free(m_storage);
		}
	}

	friend bool operator==(const Optional<ElementType>& lhs, const Optional<ElementType>& rhs)
	{
		if (lhs.isValid() != rhs.isValid()) return false;
		if (!lhs.isValid()) return true; // both unset
		return (*(ElementType*)&lhs.m_storage) == (*(ElementType*)&rhs.m_storage);
	}
	FORCEINLINE friend bool operator!=(const Optional<ElementType>& lhs, const Optional<ElementType>& rhs)
	{
		return !(lhs == rhs);
	}

	/** Return true when the value is meaningful; false if calling get() is undefined. */
	FORCEINLINE bool isValid() const { return (m_storage != nullptr); }
	FORCEINLINE explicit operator bool() const { return isValid(); }

	FORCEINLINE ElementType& get() { ASSERT(isValid()); return *m_storage; }
	FORCEINLINE const ElementType& get() const { ASSERT(isValid()); return *m_storage; }

	FORCEINLINE ElementType* operator->() { return &get(); }
	FORCEINLINE const ElementType* operator->() const { return &get(); }

	FORCEINLINE operator ElementType&() { return get(); }

private:
	FORCEINLINE void destructStorage()
	{
		typedef ElementType		OptionalDestructOptionalType;
		m_storage->OptionalDestructOptionalType::~OptionalDestructOptionalType();
	}

private:
	ElementType*	m_storage{};
};
//////////////////////////////////////////////////////////////////////////

NS_END
