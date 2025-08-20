/*!
 * \file ContainerMemory.h
 *
 * \author mattho
 * \date 2016/10/12
 *
 *
 */
#pragma once

#include "../BaseType.h"
#include "../AssertionMacros.h"
#include <type_traits>
#include <cstdlib>
#include <cstring>

#undef min
#undef max

NS_BEGIN

//////////////////////////////////////////////////////////////////////////
const std::size_t	DEFAULT_ALIGNMENT = 0;
const std::size_t	MINIMAL_ALIGNMENT = 8;

inline void* align(std::size_t alignment, std::size_t size, void*& ptr, std::size_t& space)
{
	const std::uintptr_t pn = reinterpret_cast<std::uintptr_t>(ptr);
	const std::uintptr_t aligned = (pn + alignment - 1) & ~(alignment - 1);
	const std::size_t padding = aligned - pn;
	if (space < size + padding) return nullptr;
	space -= padding;
	ptr = reinterpret_cast<void*>(aligned);
	return ptr;
}

FORCEINLINE bool is_aligned(const volatile void* ptr, const std::size_t inAlignment)
{
	return !(std::uintptr_t(ptr) & (inAlignment - 1));
}

template<typename ElementType>
INLINE_CONSTEXPR ElementType align(const ElementType inElement, std::size_t inAlignment)
{
	return (ElementType)(((std::uintptr_t)inElement + (inAlignment - 1)) & ~(inAlignment - 1));
}

template<typename ElementType>
INLINE_CONSTEXPR ElementType alignArbitrary(const ElementType inElement, std::size_t inAlignment)
{
	return (ElementType)((((std::uintptr_t)inElement + (inAlignment - 1)) / inAlignment) * inAlignment);
}
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
/**
 * Used to declare an untyped array of data with compile-time alignment.
 */
template<std::size_t tSize, std::size_t tAlignment>
struct aligned_bytes;

template<std::size_t tSize>
struct aligned_bytes<tSize, 1>
{
	uint8	m_padding[tSize];
};

#define IMPLEMENT_ALIGNED_STORAGE(alignment) \
	template<std::size_t tSize> \
	struct aligned_bytes<tSize, alignment> \
	{ \
		struct MSC_ALIGN(alignment) padding_type \
		{ \
			uint8	_pad[tSize]; \
		} GCC_ALIGN(alignment); \
		padding_type m_padding; \
	}

IMPLEMENT_ALIGNED_STORAGE(16);
IMPLEMENT_ALIGNED_STORAGE(8);
IMPLEMENT_ALIGNED_STORAGE(4);
IMPLEMENT_ALIGNED_STORAGE(2);

#undef IMPLEMENT_ALIGNED_STORAGE
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
template<typename ElementType>
class type_compatible_bytes : public aligned_bytes<sizeof(ElementType), std::alignment_of<ElementType>::value>
{};
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
namespace detail
{
	/** A type which is used to represent a type that is unknow at compile time. */
	class ContainerElement {};

	FORCEINLINE std::size_t defaultCalculateSlackShrink(std::size_t inNumElements, std::size_t inMaxElements, const std::size_t inBytesPerElement)
	{
		std::size_t retval;
		ASSERT(inNumElements < inMaxElements);

		// if the container has too much slack, shrink it to exactly fit the number of elements
		const std::size_t nCurSlackElements = inMaxElements - inNumElements;
		const std::size_t nCurSlackBytes = nCurSlackElements * inBytesPerElement;
		const bool bTooManySlackBytes = nCurSlackBytes >= 16384;
		const bool bTooManySlackElements = (3 * inNumElements) < (2 * inMaxElements);
		if ((bTooManySlackBytes || bTooManySlackElements) && (nCurSlackElements > 64 || !inNumElements))
		{
			retval = inNumElements;
		}
		else
		{
			retval = inMaxElements;
		}
		return retval;
	}

	FORCEINLINE std::size_t defaultCalculateSlackReserve(std::size_t inNumElements, const std::size_t inNumBytesPerElement)
	{
		return inNumElements;
	}

	FORCEINLINE std::size_t defaultCalculateSlackGrow(std::size_t inNumElements, std::size_t inMaxElements, const std::size_t inBytesPerElement)
	{
		ASSERT(inNumElements > inMaxElements && inNumElements > 0);

		const std::size_t nFirstAllocation = 4;

		std::size_t retval = (inMaxElements || inNumElements > nFirstAllocation)
			// allocate slack for the array proportional to its size
			? inNumElements + 3 * inNumElements / 8 + 16
			: nFirstAllocation;

		if ((int32)inNumElements > (int32)retval)
		{
			retval = (std::size_t)std::numeric_limits<int32>::max();
			ASSERT(false);
		}
		return retval;
	}

	FORCEINLINE std::size_t calculateDefaultSlack(std::size_t inNumElements, std::size_t inMaxElements, const std::size_t inBytesPerElement)
	{
		ASSERT(inNumElements > 0);
		if (inNumElements < inMaxElements)
		{
			return defaultCalculateSlackShrink(inNumElements, inMaxElements, inBytesPerElement);
		}
		else if (inNumElements > 0)
		{
			return defaultCalculateSlackGrow(inNumElements, inMaxElements, inBytesPerElement);
		}
		else
		{
			return inMaxElements;
		}
	}

	FORCEINLINE void shiftRight(void* inData, std::size_t inNumElements, std::size_t inElementIndex, std::size_t inShiftElements, const std::size_t inBytesPerElement)
	{
		if (inElementIndex < inNumElements)
		{
			std::memmove(
				(uint8*)inData + (inElementIndex + inShiftElements) * inBytesPerElement,
				(uint8*)inData + (inElementIndex)* inBytesPerElement,
				(inNumElements - inElementIndex) * inBytesPerElement);
		}
	}

	FORCEINLINE void shiftLeft(void* inData, std::size_t inNumElements, std::size_t inElementIndex, std::size_t inShiftElements, const std::size_t inBytesPerElement)
	{
		const std::size_t nStartMoveIndex = inElementIndex + inShiftElements;
		if (nStartMoveIndex < inNumElements)
		{
			std::memmove(
				(uint8*)inData + (inElementIndex)* inBytesPerElement,
				(uint8*)inData + (nStartMoveIndex)* inBytesPerElement,
				(inNumElements - nStartMoveIndex) * inBytesPerElement);
		}
	}
}
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
template<typename ElementType>
class element_trait_base
{
public:
	static const bool NeedsCopyConstructor =
		!__has_trivial_copy(ElementType) &&
		!std::is_pod<ElementType>::value;

	static const bool NeedsCopyAssignment =
		!__has_trivial_assign(ElementType) &&
		!std::is_pod<ElementType>::value;

	static const bool NeedsMoveConstructor = NeedsCopyConstructor;
	static const bool NeedsMoveAssignment = NeedsCopyAssignment;

	static const bool NeedsDestructor =
		!__has_trivial_destructor(ElementType) &&
		!std::is_pod<ElementType>::value;

	static const bool ZeroConstructable =
		std::is_enum<ElementType>::value ||
		std::is_arithmetic<ElementType>::value ||
		std::is_pointer<ElementType>::value;

	static const bool BytewiseComparable =
		std::is_enum<ElementType>::value ||
		std::is_arithmetic<ElementType>::value ||
		std::is_pointer<ElementType>::value;
};

template<typename ElementType>
class element_trait : public element_trait_base<ElementType>
{};
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
template<typename AllocatorType>
class allocation_trait_base
{
public:
	static const bool SupportsMove = false;
	static const bool IsZeroConstruct = false;
};

template<typename AllocatorType>
class allocation_trait : public allocation_trait_base<AllocatorType>
{};
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
template<typename ContainerType>
class container_trait_base
{
public:
	/** This should be overridden by every container that supports emptying its contents via a move operation. */
	static const bool MoveWillEmptyContainer = false;
};

template<typename ContainerType>
class container_trait : public container_trait_base<ContainerType>
{};
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
template<typename T, bool tIsSmall>
class parameter_type_trait
{
public:
	typedef	const T&	param_type;
	typedef	const T&	const_param_type;
};

template<typename T>
class parameter_type_trait<T, true>
{
public:
	typedef	const T		param_type;
	typedef	const T		const_param_type;
};

template<typename T>
class parameter_type_trait<T*, true>
{
public:
	typedef	T*			param_type;
	typedef	const T*	const_param_type;
};
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
template<typename T>
class parameter_trait_base
{
private:
	static const bool PassByValue =
		((sizeof(T) <= sizeof(void*) && std::is_pod<T>::value)
		|| std::is_arithmetic<T>::value || std::is_pointer<T>::value);

public:
	typedef	T			value_type;
	typedef T&			reference;
	typedef const T&	const_reference;
	typedef typename parameter_type_trait<T, PassByValue>::param_type		param_type;
	typedef typename parameter_type_trait<T, PassByValue>::const_param_type	const_param_type;
};

template<typename T>
class parameter_trait : public parameter_trait_base<T>
{ };

template<typename T>
class parameter_trait<T&>
{
public:
	typedef	T&			value_type;
	typedef T&			reference;
	typedef const T&	const_reference;
	typedef T&			param_type;
	typedef T&			const_param_type;
};

template<typename T, std::size_t tElementNum>
class parameter_trait<T [tElementNum]>
{
private:
	typedef	T			array_type[tElementNum];

public:
	typedef	const T*			value_type;
	typedef array_type&			reference;
	typedef const array_type&	const_reference;
	typedef const T*			param_type;
	typedef const T*			const_param_type;
};
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
/** Tests if a type T is bitwise-constructible from a given argument type U. */
template<typename T, typename U>
class is_bitwise_constructible
{
public:
	static_assert(
		!std::is_reference<T>::value &&
		!std::is_reference<U>::value,
		"is_bitwise_constructible is not designed to accept reference types");


	static_assert(
		std::is_same<T, typename std::remove_cv<T>::type>::value &&
		std::is_same<U, typename std::remove_cv<U>::type>::value,
		"is_bitwise_constructible is not designed to accept qualified types");

	static const bool value = false;
};

template<typename T>
class is_bitwise_constructible<T, T>
{
public:
	// Ts can always be bitwise constructed from itself if it is trivially copyable.
	static const bool value = !element_trait<T>::NeedsCopyConstructor;
};

template<typename T, typename U>
class is_bitwise_constructible<const T, U> : is_bitwise_constructible<T, U>
{};

template<typename T>
class is_bitwise_constructible<const T*, T*>
{
public:
	// Const pointers can be bitwise constructed from non-const pointers.
	static const bool value = true;
};

template<> class is_bitwise_constructible< uint8,   int8> { public: static const bool value = true; };
template<> class is_bitwise_constructible<  int8,  uint8> { public: static const bool value = true; };
template<> class is_bitwise_constructible<uint16,  int16> { public: static const bool value = true; };
template<> class is_bitwise_constructible< int16, uint16> { public: static const bool value = true; };
template<> class is_bitwise_constructible<uint32,  int32> { public: static const bool value = true; };
template<> class is_bitwise_constructible< int32, uint32> { public: static const bool value = true; };
template<> class is_bitwise_constructible<UI64,  I64> { public: static const bool value = true; };
template<> class is_bitwise_constructible< I64, UI64> { public: static const bool value = true; };
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
/** Tests if a type T is bitwise-relocatable from a given argument type U. */
template<typename T, typename U>
class can_bitwise_relocate
{
public:
	static const bool value =
		(is_bitwise_constructible<T, U>::value && !element_trait<U>::NeedsDestructor) ||
		std::is_same<T, U>::value;
};
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
namespace detail
{
	template<typename ElementType, typename ArgumentType>
	FORCEINLINE typename std::enable_if<is_bitwise_constructible<ElementType, ArgumentType>::value>::type constructElements(void* inDest, const ArgumentType* inSource, std::size_t inNumElements)
	{
		std::memcpy(inDest, inSource, inNumElements * sizeof(ArgumentType));
	}

	template<typename ElementType, typename ArgumentType>
	FORCEINLINE typename std::enable_if<!is_bitwise_constructible<ElementType, ArgumentType>::value>::type constructElements(void* inDest, const ArgumentType* inSource, std::size_t inNumElements)
	{
		ElementType* inElements = reinterpret_cast<ElementType*>(inDest);
		while (inNumElements)
		{
			new(inElements) ElementType(*inSource);
			++inElements;
			++inSource;
			--inNumElements;
		}
	}

	template<typename ElementType>
	FORCEINLINE typename std::enable_if<element_trait<ElementType>::ZeroConstructable>::type defaultConstructElements(void* inAddr, std::size_t inNumElements)
	{
		std::memset(inAddr, 0, inNumElements * sizeof(ElementType));
	}

	template<typename ElementType>
	FORCEINLINE typename std::enable_if<!element_trait<ElementType>::ZeroConstructable>::type defaultConstructElements(void* inAddr, std::size_t inNumElements)
	{
		ElementType* inElements = reinterpret_cast<ElementType*>(inAddr);
		while (inNumElements)
		{
			new(inElements) ElementType();
			++inElements;
			--inNumElements;
		}
	}

	template<typename ElementType>
	FORCEINLINE typename std::enable_if<element_trait<ElementType>::NeedsDestructor>::type destructElements(ElementType* inElements, std::size_t inNumElements)
	{
		while (inNumElements)
		{
			// We need a typedef here to prevent ElementType itself has a member called 'ElementType'
			typedef ElementType		DestructElementsElementTypeTypedef;

			inElements->DestructElementsElementTypeTypedef::~DestructElementsElementTypeTypedef();
			++inElements;
			--inNumElements;
		}
	}

	template<typename ElementType>
	FORCEINLINE typename std::enable_if<!element_trait<ElementType>::NeedsDestructor>::type destructElements(ElementType*, std::size_t)
	{
	}

	template<typename ElementType, typename SourceElementType>
	FORCEINLINE typename std::enable_if<can_bitwise_relocate<ElementType, SourceElementType>::value>::type relocateConstructElements(void* inDest, const SourceElementType* inSource, std::size_t inNumElements)
	{
		/** 
		 * We assume all containers trivial relocatability (i.e. memcpy'able) of their members,
		 * so we're going to assume that this is safe here.  However, it's not generally possible to assume this
		 * in general as objects which contain pointers/references to themselves are not safe to be trivially
		 * relocated.
		 *
		 * However, it is not yet possible to automatically infer this at compile time, so we can't enable
		 * different (i.e. safer) implementations anyway.
		 */
		std::memmove(inDest, inSource, inNumElements * sizeof(SourceElementType));
	}

	template<typename ElementType, typename SourceElementType>
	FORCEINLINE typename std::enable_if<!can_bitwise_relocate<ElementType, SourceElementType>::value>::type relocateConstructElements(void* inDest, const SourceElementType* inSource, std::size_t inNumElements)
	{
		ElementType* inElements = reinterpret_cast<ElementType*>(inDest);
		while (inNumElements)
		{
			// We need a typedef here to prevent SourceElementType itself has a member called 'SourceElementType'
			typedef SourceElementType	RelocateConstructElementsElementTypeTypedef;

			new(inElements) ElementType(*inSource);
			inSource->RelocateConstructElementsElementTypeTypedef::~RelocateConstructElementsElementTypeTypedef();

			++inElements;
			++inSource;
			--inNumElements;
		}
	}

	template<typename ElementType>
	FORCEINLINE typename std::enable_if<element_trait<ElementType>::BytewiseComparable, bool>::type isEqualElements(const ElementType* inA, const ElementType* inB, std::size_t inNumElements)
	{
		return !std::memcmp(inA, inB, inNumElements * sizeof(ElementType));
	}

	template<typename ElementType>
	FORCEINLINE typename std::enable_if<!element_trait<ElementType>::BytewiseComparable, bool>::type isEqualElements(const ElementType* inA, const ElementType* inB, std::size_t inNumElements)
	{
		while (inNumElements)
		{
			if (!(*inA == *inB))
			{
				return false;
			}

			++inA;
			++inB;
			--inNumElements;
		}
		return true;
	}
}
//////////////////////////////////////////////////////////////////////////

NS_END
