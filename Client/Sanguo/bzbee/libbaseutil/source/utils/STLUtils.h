/*!
 * \file STLUtils.h
 *
 * \author mattho
 * \date 2017/11/30
 *
 *
 */
#pragma once

#include <BaseType.h>
#include <AssertionMacros.h>
#include <algorithm>
#include <utility>

namespace std
{
	namespace detail
	{
		template<typename, typename T>
		struct has_find
		{
			static_assert(std::integral_constant<T, false>::value, "Second template parameter needs to be of function type.");
		};

		template<typename ClassType, typename ReturnType, typename... ParamTypes>
		struct has_find<ClassType, ReturnType(ParamTypes...)>
		{
		private:
			template<typename T>
			static auto check(T*)
				-> typename std::is_same<decltype(std::declval<T>().find(std::declval<ParamTypes>()...)), ReturnType>::type;

			template<typename>
			static std::false_type check(...);

			typedef decltype(check<ClassType>(0)) type;

		public:
			static const bool value = type::value;
		};
	}

	template<typename ContainerType>
	FORCEINLINE void purge(ContainerType& inContainer, std::size_t inExpectedNumElements = 0)
	{
		{
			ContainerType purgeDummy;
			std::swap(inContainer, purgeDummy);
		}

		if (inExpectedNumElements > 0)
		{
			inContainer.reserve(inExpectedNumElements);
		}
	}

	template<typename ContainerType, typename KeyType>
	FORCEINLINE typename std::enable_if<
		detail::has_find<ContainerType, typename ContainerType::iterator(const KeyType&)>::value, bool>
	::type contains(const ContainerType& inContainer, const KeyType& inKey)
	{
		return (inContainer.find(inKey) != inContainer.end());
	}

	template<typename ContainerType, typename KeyType>
	FORCEINLINE typename std::enable_if<
		!detail::has_find<ContainerType, typename ContainerType::iterator(const KeyType&)>::value, bool>
	::type contains(const ContainerType& inContainer, const KeyType& inKey)
	{
		const auto end_iter = inContainer.end();
		return std::find(inContainer.begin(), end_iter, inKey) != end_iter;
	}

	template<typename MapperType, typename KeyType>
	FORCEINLINE typename MapperType::mapped_type find_ref(const MapperType& inContainer, const KeyType& inKey)
	{
		auto found_iter = inContainer.find(inKey);
		return (found_iter != inContainer.end()) ? found_iter->second : typename MapperType::mapped_type();
	}

	template<typename MapperType, typename KeyType>
	FORCEINLINE typename MapperType::mapped_type* find_uncheck(MapperType& inContainer, const KeyType& inKey)
	{
		auto found_iter = inContainer.find(inKey);
		return (found_iter != inContainer.end()) ? &found_iter->second : nullptr;
	}

	template<typename MapperType, typename KeyType>
	FORCEINLINE typename MapperType::mapped_type const* find_uncheck(const MapperType& inContainer, const KeyType& inKey)
	{
		auto found_iter = inContainer.find(inKey);
		return (found_iter != inContainer.end()) ? &found_iter->second : nullptr;
	}

	/** Returns a reference to the value associated with a specified key. */
	template<typename MapperType, typename KeyType>
	FORCEINLINE typename MapperType::mapped_type& find_checked(MapperType& inContainer, const KeyType& inKey)
	{
		auto found_iter = inContainer.find(inKey);
		ASSERT(found_iter != inContainer.end());
		return found_iter->second;
	}

	template<typename MapperType, typename KeyType>
	FORCEINLINE typename MapperType::mapped_type const& find_checked(const MapperType& inContainer, const KeyType& inKey)
	{
		auto found_iter = inContainer.find(inKey);
		ASSERT(found_iter != inContainer.end());
		return found_iter->second;
	}

	template<typename MapperType, typename KeyType>
	typename MapperType::mapped_type& find_or_new(MapperType& inContainer, const KeyType& inKey)
	{
		auto found_iter = inContainer.find(inKey);
		if (found_iter != inContainer.end())
		{
			return found_iter->second;
		}
		
		auto insert_result = inContainer.emplace(inKey, typename MapperType::mapped_type());
		return insert_result.first->second;
	}

// 	template<typename MapperType, typename KeyType>
// 	FORCEINLINE typename MapperType::mapped_type& find_or_new(MapperType& inContainer, KeyType&& inKey)
// 	{
// 		return find_or_new(inContainer, inKey);
// 	}

	template<typename MapperType>
	FORCEINLINE void merge(MapperType& inOutMap, const MapperType& inOther)
	{
		for (const auto& iter : inOther)
		{
			if (inOutMap.find(iter.first) == inOutMap.end())
			{
				inOutMap.emplace(iter);
			}
		}
	}

	/** Generates an array from the keys in this mapper. */
	template<typename MapperType, typename ArrayType>
	FORCEINLINE void get_keys(const MapperType& inContainer, ArrayType& outArray)
	{
		outArray.clear();
		outArray.reserve(inContainer.size());
		for (auto& iter : inContainer)
		{
			outArray.emplace_back(iter.first);
		}
	}

	template<typename MapperType, typename Comparer>
	void sort_by_value(MapperType& inContainer, const Comparer& inPredicate)
	{
	}

	template<typename MapperType, typename Comparer>
	void sort_by_key(MapperType& inContainer, const Comparer& inPredicate)
	{
	}

	template<typename _Elem, typename _Traits, typename _Alloc>
	void replace_substr(
		basic_string<_Elem, _Traits, _Alloc>& inOutput,
		const _Elem* inFind, size_t inFindCount,
		const _Elem* inReplace, size_t inReplaceCount)
	{
		size_t index = 0;
		while (true)
		{
			// Locate the substring to replace
			index = inOutput.find(inFind, index);
			if (index == basic_string<_Elem, _Traits, _Alloc>::npos)
			{
				break;
			}

			// Make the replacement
			inOutput.replace(index, inFindCount, inReplace);

			// Advance index forward so the next iteration doesn't pick it up as well
			index += inReplaceCount;
			if (index > inOutput.length())
			{
				break;
			}
		}
	}
}
