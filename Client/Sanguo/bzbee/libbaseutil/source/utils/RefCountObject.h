/*!
 * \file RefCountObject.h
 *
 * \author mattho
 * \date 2016/10/18
 *
 *
 */
#pragma once

#include "../BaseType.h"
#include "../AssertionMacros.h"
#include <memory>

NS_BEGIN

//////////////////////////////////////////////////////////////////////////
class RefCountedInterface
{
public:
	virtual uint32 incRef() const = 0;
	virtual uint32 release() = 0;
	virtual uint32 getRefCount() const = 0;
};
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
class RefCountedObject
{
public:
	RefCountedObject()
	{}

	virtual ~RefCountedObject()
	{
		ASSERT(!m_numRefs);
	}

	FORCEINLINE uint32 incRef() const
	{
		return uint32(++m_numRefs);
	}

	uint32 release()
	{
		const uint32 nRefs = uint32(--m_numRefs);
		if (nRefs == 0)
		{
			delete this;
		}
		ASSERT(nRefs >= 0);
		return nRefs;
	}

	FORCEINLINE uint32 getRefCount() const
	{
		return uint32(m_numRefs);
	}

private:
	mutable	int32	m_numRefs{};
};
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
template<typename ReferencedType>
class RefCountPtr
{
	typedef ReferencedType*	ReferenceType;

public:
	FORCEINLINE RefCountPtr()
		: m_ptr(nullptr)
	{}
	RefCountPtr(ReferencedType* inPtr, bool bIncRef = true)
		: m_ptr(inPtr)
	{
		if (m_ptr && bIncRef) m_ptr->incRef();
	}
	RefCountPtr(const RefCountPtr& inOther)
		: m_ptr(inOther.m_ptr)
	{
		if (m_ptr) m_ptr->incRef();
	}

	~RefCountPtr()
	{
		if (m_ptr) m_ptr->release();
	}

	RefCountPtr& operator=(ReferencedType* inPtr)
	{
		if (inPtr != m_ptr)
		{
			ReferencedType* prevPtr = m_ptr;
			m_ptr = inPtr;

			if (m_ptr) m_ptr->incRef();
			if (prevPtr) prevPtr->release();
		}
		return *this;
	}

	FORCEINLINE RefCountPtr& operator=(const RefCountPtr& inOther)
	{
		return *this = inOther.m_ptr;
	}

	RefCountPtr& operator=(RefCountPtr&& inOther)
	{
		if (this != &inOther)
		{
			ReferencedType* prevPtr = m_ptr;
			m_ptr = inOther.m_ptr;

			inOther.m_ptr = nullptr;
			if (prevPtr) prevPtr->release();
		}
		return *this;
	}

	FORCEINLINE ReferencedType* operator->() const { return m_ptr; }
	FORCEINLINE operator ReferenceType() const { return m_ptr; }
	FORCEINLINE ReferencedType* getReference() const { return m_ptr; }

	FORCEINLINE ReferencedType** getInitReference()
	{
		*this = nullptr;
		return &m_ptr;
	}

	FORCEINLINE bool isValid() const { return (m_ptr != nullptr); }
	FORCEINLINE void safeRelease() { *this = nullptr; }

	uint32 getRefCount() const
	{
		uint32 retval = 0;
		if (m_ptr)
		{
			retval = m_ptr->getRefCount();
			ASSERT(retval > 0);
		}
		return retval;
	}

	FORCEINLINE void swap(RefCountPtr& inOther)
	{
		ReferencedType* ptr = m_ptr;
		m_ptr = inOther.m_ptr;
		inOther.m_ptr = ptr;
	}

	static FORCEINLINE uint32 getTypeHash(const RefCountPtr& inPtr)
	{
		return std::hash<ReferencedType*>()(inPtr.m_ptr);
	}

private:
	ReferencedType*		m_ptr{};
};
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
template<typename ReferencedType>
FORCEINLINE bool operator==(const RefCountPtr<ReferencedType>& inA, const RefCountPtr<ReferencedType>& inB)
{
	return inA.getReference() == inB.getReference();
}

template<typename ReferencedType>
FORCEINLINE bool operator==(const RefCountPtr<ReferencedType>& inA, ReferencedType* inB)
{
	return inA.getReference() == inB;
}

template<typename ReferencedType>
FORCEINLINE bool operator==(ReferencedType* inA, const RefCountPtr<ReferencedType>& inB)
{
	return inA == inB.getReference();
}
//////////////////////////////////////////////////////////////////////////

NS_END

//////////////////////////////////////////////////////////////////////////
namespace std
{
	template<typename ReferencedType>
	FORCEINLINE void swap(bzbee::RefCountPtr<ReferencedType>& lhs, bzbee::RefCountPtr<ReferencedType>& rhs)
	{
		ReferencedType* lhsPtr = lhs.m_ptr;
		lhs.m_ptr = rhs.m_ptr;
		rhs.m_ptr = lhsPtr;
	}

	template<typename ReferencedType>
	struct hash<bzbee::RefCountPtr<ReferencedType>>
	{
		FORCEINLINE std::size_t operator()(const bzbee::RefCountPtr<ReferencedType>& key) const
		{
			return bzbee::RefCountPtr<ReferencedType>::getTypeHash(key);
		}
	};
}
//////////////////////////////////////////////////////////////////////////
