/*!
 * \file EnumUtil.h
 *
 * \author mattho
 * \date 2016/08/02
 *
 *
 */
#pragma once
#include "BaseType.h"
#include <array>

NS_BEGIN

namespace detail
{
	template<typename TEnumType, typename TOutType>
	class EnumContiguousIterator
	{
	public:
		typedef __underlying_type(TEnumType) IntType;

		inline explicit EnumContiguousIterator(TEnumType inEnumValue)
			: m_enumValue((IntType)inEnumValue)
		{}

		inline EnumContiguousIterator& operator++()
		{
			++m_enumValue;
			return *this;
		}

		inline TOutType operator*() const
		{
			return (TOutType)m_enumValue;
		}

	private:
		inline friend bool operator!=(const EnumContiguousIterator& Lhs, const EnumContiguousIterator& Rhs)
		{
			return Lhs.m_enumValue != Rhs.m_enumValue;
		}

	protected:
		IntType		m_enumValue;
	};

	template<typename TEnumType, typename TOutType>
	class EnumValueArrayIterator
	{
	public:
		inline explicit EnumValueArrayIterator(const TEnumType* inEnumPtr)
			: m_enumPtr(inEnumPtr)
		{}

		inline EnumValueArrayIterator& operator++()
		{
			++m_enumPtr;
			return *this;
		}

		inline TOutType operator*() const
		{
			return (TOutType)*m_enumPtr;
		}

	private:
		inline friend bool operator!=(const EnumValueArrayIterator& Lhs, const EnumValueArrayIterator& Rhs)
		{
			return Lhs.m_enumPtr != Rhs.m_enumPtr;
		}

	private:
		const TEnumType* m_enumPtr;
	};

	template<typename TEnumType, typename TRangeType, int32 RangeType>
	struct EnumRangeImpl
	{
		static_assert(sizeof(TEnumType) == 0, "Unknown enum type - use one of the ENUM_RANGE macros to define iteration semantics for your enum type.");
	};

	template<typename TEnumType, typename TRangeType>
	class EnumRangeImpl<TEnumType, TRangeType, 0>
	{
	protected:
		typedef detail::EnumContiguousIterator<TEnumType, TRangeType>	EnumIterator;

		friend EnumIterator begin(const EnumRangeImpl& inRange) { return EnumIterator((TEnumType)0); }
		friend EnumIterator end(const EnumRangeImpl& inRange) { return EnumIterator(inRange.m_count); }

		TEnumType		m_count;
	};

	template<typename TEnumType, typename TRangeType>
	class EnumRangeImpl<TEnumType, TRangeType, 1>
	{
	protected:
		typedef detail::EnumValueArrayIterator<TEnumType, TRangeType>	EnumIterator;

		friend EnumIterator begin(const EnumRangeImpl& inRange) { return EnumIterator((TEnumType)0); }
		friend EnumIterator end(const EnumRangeImpl& inRange) { return EnumIterator(inRange.m_count); }
	};
}

template<typename TEnumType>
class EnumRange : public detail::EnumRangeImpl<TEnumType, TEnumType, 0>
{
public:
	inline explicit EnumRange(TEnumType inEnumCount)
	{
		this->m_count = inEnumCount;
	}
};

template<typename TEnumType>
class EnumIntRange : public detail::EnumRangeImpl<TEnumType, __underlying_type(TEnumType), 0>
{
public:
	inline explicit EnumIntRange(TEnumType inEnumCount)
	{
		this->m_count = inEnumCount;
	}
};

template<typename TEnumType>
inline EnumRange<TEnumType> enumRangeByCount(TEnumType inEnumCount)
{
	return EnumRange<TEnumType>(inEnumCount);
}

template<typename TEnumType>
inline EnumIntRange<TEnumType> enumIntRangeByCount(TEnumType inEnumCount)
{
	return EnumIntRange<TEnumType>(inEnumCount);
}

template<typename... TEnumParams>
inline std::array<typename std::common_type<TEnumParams...>::type, sizeof... (TEnumParams)> enumRangeByValues(TEnumParams&&... inArgs)
{
	return { std::forward<TEnumParams>(inArgs)... };
}

NS_END

#if defined(_MSC_VER) && (_MSC_VER < 1900)
#define enumInt(x)		(static_cast<int>(x))

#else
template<typename TEnumType>
inline constexpr __underlying_type(TEnumType) enumInt(TEnumType inEnumValue)
{
	return (__underlying_type(TEnumType))inEnumValue;
}
#endif

// defines all bitwise operators for enum classes so it can be (mostly) used as a regular flags enum
#define ENUM_CLASS_FLAGS_IMPL(EnumType, Prefix) \
	Prefix inline EnumType& operator|=(EnumType& lhs, EnumType rhs) { return lhs = (EnumType)( (__underlying_type(EnumType))lhs | (__underlying_type(EnumType))rhs ); } \
	Prefix inline EnumType& operator&=(EnumType& lhs, EnumType rhs) { return lhs = (EnumType)( (__underlying_type(EnumType))lhs & (__underlying_type(EnumType))rhs ); } \
	Prefix inline EnumType& operator^=(EnumType& lhs, EnumType rhs) { return lhs = (EnumType)( (__underlying_type(EnumType))lhs ^ (__underlying_type(EnumType))rhs ); } \
	Prefix INLINE_CONSTEXPR EnumType operator|(EnumType lhs, EnumType rhs) { return (EnumType)( (__underlying_type(EnumType))lhs | (__underlying_type(EnumType))rhs ); } \
	Prefix INLINE_CONSTEXPR EnumType operator&(EnumType lhs, EnumType rhs) { return (EnumType)( (__underlying_type(EnumType))lhs & (__underlying_type(EnumType))rhs ); } \
	Prefix INLINE_CONSTEXPR EnumType operator^(EnumType lhs, EnumType rhs) { return (EnumType)( (__underlying_type(EnumType))lhs ^ (__underlying_type(EnumType))rhs ); } \
	Prefix INLINE_CONSTEXPR EnumType operator~(EnumType value) { return (EnumType)~(__underlying_type(EnumType))value; } \
	Prefix INLINE_CONSTEXPR bool operator!(EnumType value) { return !(__underlying_type(EnumType))value; }
	//Prefix INLINE_CONSTEXPR bool operator&&(EnumType value, EnumType mask) { return ((__underlying_type(EnumType))value & (__underlying_type(EnumType))mask) == (__underlying_type(EnumType))mask; } \
	//Prefix INLINE_CONSTEXPR bool operator||(EnumType value, EnumType mask) { return ((__underlying_type(EnumType))value | (__underlying_type(EnumType))mask) == (__underlying_type(EnumType))mask; }
#undef ENUM_CLASS_FLAGS
#undef ENUM_CLASS_FLAGS_INCLASS
#define ENUM_CLASS_FLAGS(EnumType)			ENUM_CLASS_FLAGS_IMPL(EnumType,)
#define ENUM_CLASS_FLAGS_INCLASS(EnumType)	ENUM_CLASS_FLAGS_IMPL(EnumType, friend)
