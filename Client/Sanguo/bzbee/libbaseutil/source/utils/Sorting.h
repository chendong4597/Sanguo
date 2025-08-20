/*!
 * \file Sorting.h
 *
 * \author mattho
 * \date 2016/10/12
 *
 *
 */
#pragma once

#include "../BaseType.h"
#include "../AssertionMacros.h"
#include <functional>
#include <cstdint>
#include <algorithm>

NS_BEGIN

namespace detail
{
	/**
	 * Sort elements using user defined predicate class. The sort is unstable, meaning that the ordering of equal items is not necessarily preserved.
	 * This is the internal sorting function used by Sort overrides.
	 *
	 * \param First		pointer to the first element to sort
	 * \param Num		the number of items to sort
	 * \param Predicate	predicate class
	 */
	template<typename T, typename PredicateType>
	void sortInternal(T* First, const std::size_t inNumElements, const PredicateType& Predicate)
	{
		//std::stable_sort(First, First + inNumElements, Predicate);
		//return;
		struct ExecStack
		{
			T* Min;
			T* Max;
		};

		if (inNumElements <= 1) return;

		ExecStack RecursionStack[32] = { { First, First + inNumElements - 1 } }, Current, Inner;
		for (ExecStack* StackTop = RecursionStack; StackTop >= RecursionStack; --StackTop) //-V625
		{
			Current = *StackTop;

		Loop:
			std::intptr_t Count = Current.Max - Current.Min + 1;
			if (Count <= 8)
			{
				// Use simple bubble-sort.
				while (Current.Max > Current.Min)
				{
					T *Max, *Item;
					for (Max = Current.Min, Item = Current.Min + 1; Item <= Current.Max; Item++)
					{
						if (Predicate(*Max, *Item))
						{
							Max = Item;
						}
					}
					std::swap(*Max, *Current.Max--);
				}
			}
			else
			{
				// Grab middle element so sort doesn't exhibit worst-cast behavior with presorted lists.
				std::swap(Current.Min[Count / 2], Current.Min[0]);

				// Divide list into two halves, one with items <=Current.Min, the other with items >Current.Max.
				Inner.Min = Current.Min;
				Inner.Max = Current.Max + 1;
				for (;;)
				{
					while (++Inner.Min <= Current.Max && !Predicate(*Current.Min, *Inner.Min));
					while (--Inner.Max >  Current.Min && !Predicate(*Inner.Max, *Current.Min));
					if (Inner.Min > Inner.Max)
					{
						break;
					}
					std::swap(*Inner.Min, *Inner.Max);
				}
				std::swap(*Current.Min, *Inner.Max);

				// Save big half and recurse with small half.
				if (Inner.Max - 1 - Current.Min >= Current.Max - Inner.Min)
				{
					if (Current.Min + 1 < Inner.Max)
					{
						StackTop->Min = Current.Min;
						StackTop->Max = Inner.Max - 1;
						StackTop++;
					}
					if (Current.Max>Inner.Min)
					{
						Current.Min = Inner.Min;
						goto Loop;
					}
				}
				else
				{
					if (Current.Max > Inner.Min)
					{
						StackTop->Min = Inner.Min;
						StackTop->Max = Current.Max;
						StackTop++;
					}
					if (Current.Min + 1 < Inner.Max)
					{
						Current.Max = Inner.Max - 1;
						goto Loop;
					}
				}
			}
		}
	}
	//////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////
	/** Helper class for dereferencing pointer types in Sort function */
	template<typename T, typename PredicateType>
	struct DereferenceWrapper
	{
		DereferenceWrapper(const PredicateType& inPredicate)
			: Predicate(inPredicate)
		{}

		/** Pass through for non-pointer types */
		FORCEINLINE bool operator()(T& inA, T& inB) { return Predicate(inA, inB); }
		FORCEINLINE bool operator()(const T& inA, const T& inB) const { return Predicate(inA, inB); }

		const PredicateType& Predicate;
	};

	template<typename T, typename PredicateType>
	struct DereferenceWrapper<T*, PredicateType>
	{
		DereferenceWrapper(const PredicateType& inPredicate)
			: Predicate(inPredicate)
		{}

		/** Dereferennce pointers */
		FORCEINLINE bool operator()(T* inA, T* inB) const { return Predicate(*inA, *inB); }

		const PredicateType& Predicate;
	};

	template<typename T, typename PredicateType>
	FORCEINLINE void sort(T* First, const std::size_t nCount, const PredicateType& Predicate)
	{
		sortInternal(First, nCount, DereferenceWrapper<T, PredicateType>(Predicate));
	}

	template<typename T, typename PredicateType>
	FORCEINLINE void sort(T** First, const std::size_t nCount, const PredicateType& Predicate)
	{
		sortInternal(First, nCount, DereferenceWrapper<T*, PredicateType>(Predicate));
	}
	//////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////
	template<typename T>
	FORCEINLINE void sort(T* First, const std::size_t nCount)
	{
		sortInternal(First, nCount, DereferenceWrapper<T, std::less<T> >(std::less<T>()));
	}

	template<typename T>
	FORCEINLINE void sort(T** First, const std::size_t nCount)
	{
		sortInternal(First, nCount, DereferenceWrapper<T*, std::less<T> >(std::less<T>()));
	}
	//////////////////////////////////////////////////////////////////////////
}

NS_END
