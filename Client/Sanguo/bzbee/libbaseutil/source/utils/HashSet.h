/*!
 * \file HashSet.h
 *
 * \author mattho
 * \date 2018/01/02
 *
 *
 */
#pragma once

#include "DynamicArray.h"

NS_BEGIN

//////////////////////////////////////////////////////////////////////////
template<typename ElementType, typename KeyType, bool tAllowDuplicateKeys>
class BaseKeyFunc
{
public:
	typedef KeyType		key_type;
	typedef	typename parameter_trait<KeyType>::param_type		key_param;
	typedef	typename parameter_trait<ElementType>::param_type	value_param;

	static const bool AllowDuplicateKeys = tAllowDuplicateKeys;
};

template<typename ElementType, bool tAllowDuplicateKeys = false>
class DefaultKeyFunc : private BaseKeyFunc<ElementType, ElementType, tAllowDuplicateKeys>
{
public:
	typedef typename parameter_trait<ElementType>::param_type	key_param;
	typedef typename parameter_trait<ElementType>::param_type	value_param;

	static FORCEINLINE key_param getKey(value_param inElement)
	{
		return inElement;
	}

	static FORCEINLINE bool equals(key_param inA, key_param inB)
	{
		return (inA == inB);
	}

	static FORCEINLINE getKeyHash(key_param inKey)
	{
		return std::hash()(inKey);
	}
};
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
class DefaultSetAllocator
{};
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
template<typename T, typename KeyFunc = DefaultKeyFunc<T>, typename Allocator = DefaultSetAllocator>
class hash_set;
//////////////////////////////////////////////////////////////////////////

NS_END
