/*!
 * \file TupleArray.h
 *
 * \author mattho
 * \date 2018/05/17
 *
 *
 */
#pragma once

#include <BaseType.h>
#include <utility>
#include <tuple>
#include <bitset>

NS_BEGIN

//////////////////////////////////////////////////////////////////////////
namespace detail
{
#if 0
	template<std::size_t...>
	struct integer_sequence
	{ };

	template<std::size_t tIndex, std::size_t... tIndices>
	struct expander : expander<tIndex - 1, tIndex - 1, tIndices...>
	{ };

	template<std::size_t... tIndices>
	struct expander<0, tIndices...>
	{
		using sequence_type = integer_sequence<tIndices...>;
	};

	template<std::size_t tElements>
	using make_index_sequence = typename expander<tElements>::sequence_type;

	template<std::size_t... tIndices>
	using index_sequence = integer_sequence<tIndices...>;

#else
	template<std::size_t tElements>
	using make_index_sequence = std::make_index_sequence<tElements>;

	template<std::size_t... tIndices>
	using index_sequence = std::index_sequence<tIndices...>;
#endif

	template<typename T, std::size_t tIndex>
	using depend_on = T;

	template<typename T, std::size_t tElements,
		typename Indices = make_index_sequence<tElements>>
	struct repeat;

	template<typename T, std::size_t tElements, std::size_t... tIndices>
	struct repeat<T, tElements, index_sequence<tIndices...>>
	{
		using type = std::tuple<depend_on<T, tIndices>...>;
	};

	template<typename TupleType, std::size_t... tIndices>
	auto make_bitset(const TupleType& inTuples, index_sequence<tIndices...>)
		-> std::bitset<sizeof...(tIndices)>
	{
		std::bitset<sizeof...(tIndices)> OutValue;
		auto index = sizeof...(tIndices);
		const bool expanded[] = { true, (OutValue.set(--index, std::get<tIndices>(inTuples)), true)... };
		return OutValue;
	}
}
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
template<typename T, std::size_t tElements>
using tuple_array = typename detail::repeat<T, tElements>::type;
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
template<typename TupleType,
	typename Indices = detail::make_index_sequence<std::tuple_size<TupleType>::value>>
FORCEINLINE auto make_bitset(const TupleType& inTuples)
	-> decltype(detail::make_bitset(inTuples, Indices()))
{
	return detail::make_bitset(inTuples, Indices());
}
//////////////////////////////////////////////////////////////////////////

NS_END
