/*!
 * \file BaseType.h
 *
 * \author zjhlogo zjhlogo@gmail.com
 * \date 04/14/2016
 *
 * 
 */
#pragma once
#include "BaseUtil_Config.h"
#include "BaseTypeDefine.h"
#include "CompileSpec.h"

#include <string>
#include <vector>
#include <set>
#include <map>

NS_BEGIN





//static const uint32				  INDEX_NONE = (uint32)(-1);

class EnumClassHash
{
public:
	template <typename T>
	std::size_t operator()(T t) const
	{
		return static_cast<std::size_t>(t);
	}
};

class Color
{
public:
	static const Color WHITE;
	static const Color BLACK;
	static const Color GRAY;
	static const Color RED;
	static const Color GREEN;
	static const Color BLUE;
	static const Color YELLOW;
	static const Color CYAN;
	static const Color PURPLE;

public:
	Color(){};
	Color(uint8 r, uint8 g, uint8 b, uint8 a) :m_r(r), m_g(g), m_b(b), m_a(a) {};

	union
	{
		struct
		{
			uint8 m_r;
			uint8 m_g;
			uint8 m_b;
			uint8 m_a;
		};
		uint32 m_data{ 0xFFFFFFFF };
	};

	FORCEINLINE friend bool operator==(const Color& lhs, const Color& rhs)
	{
		return (lhs.m_data == rhs.m_data);
	}

	FORCEINLINE friend bool operator!=(const Color& lhs, const Color& rhs)
	{
		return (lhs.m_data != rhs.m_data);
	}

	friend std::ostream& operator<<(std::ostream& os, const Color& color);
	friend std::istream& operator>>(std::istream& is, Color& color);

public:
	static glm::vec4 colorToVector(const Color& color);
	static Color channelListToColor(const IntegerList& channelList);
	static Color vectorToColor(const glm::vec4& vec);
	static Color vectorToColor(const glm::vec3& vec);

};

typedef std::vector<Color> ColorList;

/** Structure for integer rectangles in 2-d space. */
class IntRect
{
public:
	FORCEINLINE IntRect()
		: min(0, 0)
		, max(0, 0)
	{ }
	FORCEINLINE IntRect(int32 x0, int32 y0, int32 x1, int32 y1)
		: min(x0, y0)
		, max(x1, y1)
	{ }
	FORCEINLINE IntRect(const glm::ivec2& inMin, const glm::ivec2& inMax)
		: min(inMin)
		, max(inMax)
	{ }

	FORCEINLINE int32 getWidth() const { return (max.x - min.x); }
	FORCEINLINE int32 getHeight() const { return (max.y - min.y); }

	FORCEINLINE int32 getArea() const
	{
		return (max.x - min.x) * (max.y - min.y);
	}

	FORCEINLINE bool contains(const glm::ivec2& inPoint)
	{
		return (inPoint.x >= min.x && inPoint.x < max.x && inPoint.y >= min.y && inPoint.y < max.y);
	}

	FORCEINLINE bool operator==(const IntRect& inOther) const
	{
		return min == inOther.min && max == inOther.max;
	}
	FORCEINLINE bool operator!=(const IntRect& inOther) const
	{
		return min != inOther.min || max != inOther.max;
	}

	FORCEINLINE IntRect& operator*=(int32 inScale)
	{
		min *= inScale;
		max *= inScale;
		return *this;
	}

	FORCEINLINE IntRect& operator+=(const glm::ivec2& inPoint)
	{
		min += inPoint;
		max += inPoint;
		return *this;
	}

	FORCEINLINE IntRect& operator-=(const glm::ivec2& inPoint)
	{
		min -= inPoint;
		max -= inPoint;
		return *this;
	}

public:
	/** Holds the first pixel line/row (like in Win32 RECT). */
	glm::ivec2	min;
	/** Holds the last pixel line/row (like in Win32 RECT). */
	glm::ivec2	max;
};

class FloatRect
{
public:
	FORCEINLINE FloatRect()
		: min(0.0, 0.0)
		, max(0.0, 0.0)
	{ }
	FORCEINLINE FloatRect(float left, float bottom, float right, float top)
		: min(left, bottom)
		, max(right, top)
	{ }
	FORCEINLINE FloatRect(const glm::vec2& inMin, const glm::vec2& inMax)
		: min(inMin)
		, max(inMax)
	{ }

	FORCEINLINE float getWidth() const { return (max.x - min.x); }
	FORCEINLINE float getHeight() const { return (max.y - min.y); }

	FORCEINLINE float getArea() const
	{
		return (max.x - min.x) * (max.y - min.y);
	}

	FORCEINLINE bool contains(const glm::vec2& inPoint)
	{
		return (inPoint.x >= min.x && inPoint.x < max.x && inPoint.y >= min.y && inPoint.y < max.y);
	}

	FORCEINLINE bool intersect(const FloatRect& other)
	{
		return !(min.x > other.max.x || min.y > other.max.y
			  || max.x < other.min.x || max.y < other.min.y);
	}

public:
	glm::vec2 min;
	glm::vec2 max;
};

template<typename T>
using ref_type = T&;

template<typename T>
using ptr_type = T*;

/**
 * Computes the base 2 logarithm for an integer value that is greater than 0.
 * The result is rounded down to the nearest integer.
 */
template<typename ValueType>
struct FloorLog2Math;

// http://codinggorilla.domemtech.com/?p=81 or http://en.wikipedia.org/wiki/Binary_logarithm
// but modified to return 0 for a input value of 0
// 686ms on test data
template<> struct FloorLog2Math<uint32>
{
	static inline uint32 apply(uint32 inValue)
	{
		uint32 pos = 0;
		if (inValue >= (1 << 16)) { inValue >>= 16; pos += 16; }
		if (inValue >= (1 <<  8)) { inValue >>=  8; pos +=  8; }
		if (inValue >= (1 <<  4)) { inValue >>=  4; pos +=  4; }
		if (inValue >= (1 <<  2)) { inValue >>=  2; pos +=  2; }
		if (inValue >= (1 <<  1)) { pos += 1; }
		return (inValue == 0) ? 0 : pos;
	}
};

template<> struct FloorLog2Math<UI64>
{
	static inline UI64 apply(UI64 inValue)
	{
		UI64 pos = 0;
		if (inValue >= (1ull << 32)) { inValue >>= 32; pos += 32; }
		if (inValue >= (1ull << 16)) { inValue >>= 16; pos += 16; }
		if (inValue >= (1ull <<  8)) { inValue >>=  8; pos +=  8; }
		if (inValue >= (1ull <<  4)) { inValue >>=  4; pos +=  4; }
		if (inValue >= (1ull <<  2)) { inValue >>=  2; pos +=  2; }
		if (inValue >= (1ull <<  1)) { pos += 1; }
		return (inValue == 0) ? 0 : pos;
	}
};

static FORCEINLINE uint32 floorLog2(uint32 inValue)
{
	return FloorLog2Math<uint32>::apply(inValue);
}

template<typename ValueType
	, typename = typename std::enable_if<std::is_integral<ValueType>::value>::type>
static FORCEINLINE ValueType getAndClearNextBit(ValueType& inBits)
{
	// Scan for the lowest set bit, compute its index, clear it in the set of dirty bits
	const ValueType LowestBitMask = (inBits) & (-(typename std::make_signed<ValueType>::type)inBits);
	const ValueType BitIndex = FloorLog2Math<ValueType>::apply(LowestBitMask);
	inBits ^= LowestBitMask;
	return BitIndex;
}

NS_END

#define METHOD(name) name
#define LUA(name) name

extern const bzbee::tstring locateGetTranslation(const bzbee::tstring& key);
extern const std::wstring locateGetTranslationW(const bzbee::tstring& key);

#ifndef SAFE_DELETE
#define SAFE_DELETE(x) if (x) {delete x; x=nullptr;}
#endif
#define SAFE_DELETE_ARRAY(x) if (x) {delete[] x; x=nullptr;}
#define SAFE_DELETE_AND_TERMINATE(x) if (x) {x->terminate(); delete x; x=nullptr;}

#ifndef SAFE_RELEASE
#define SAFE_RELEASE(x) if (x) {x->release(); x=nullptr;}
#endif

#define PP_CAT(a, b) PP_CAT_I(a, b)
#define PP_CAT_I(a, b) PP_CAT_II(~, a ## b)
#define PP_CAT_II(p, res) res

#define _LC(x) locateGetTranslation(x)
#define _WLC(x) locateGetTranslationW(x)

namespace std
{
	template<>
	struct less<glm::ivec2>
	{
		bool operator()(const glm::ivec2& _Left, const glm::ivec2& _Right) const
		{
			return (static_cast<bzbee::UI64>(_Left.x) << 32) + _Left.y < (static_cast<bzbee::UI64>(_Right.x) << 32) + _Right.y;
		}
	};
}

#ifdef _MSC_VER
#pragma warning(disable: 4351) // disable warning 4351, happened under visual studio "new behavior: elements of array '***' will be default initialized"
#pragma warning(disable: 4819)
#endif

//bit operation
#ifndef BIT
#define BIT( num )				( 1ULL << ( num ) )
#endif

#define bits_size(name) (BITS_SIZE_##name)
#define bits_offset(name) (BITS_OFFSET_##name)
#define bits_mask(name) (BITS_MASK_##name)
#define bits_min(name) (BITS_MIN_##name)
#define bits_max(name) (BITS_MAX_##name)

#define get_bits_mask(bitoffset, bitsize) (((1ULL << (bitsize)) - 1) << (bitoffset))
#define next_offset(preoffset, prebitssize) ((preoffset) + (prebitssize))
#define decode_bits(name, value) ((((value) & bits_mask(name)) >> bits_offset(name)))
#define encode_bits(name, bitsvalue) (((bitsvalue) << bits_offset(name)) & bits_mask(name))
#define clear_bits(mask, value) ((value) & ~(mask))
#define modify_bits(value, name, bitsvalue) ((clear_bits(name, value) | encode_bits(name, bitsvalue)))
#define bits_in_range(name, value) (((value) >= bits_min(name)) && (value) <= bits_max(name))

#define DEFINE_BASE_BITS(prefix, name, bitssize) \
	prefix bits_offset(name) = 0;\
	prefix bits_size(name) = (bitssize);\
	prefix bits_mask(name) = get_bits_mask(0, (bitssize));\
	prefix name = bits_mask(name); \
	prefix bits_min(name) = encode_bits(name, 1);\
	prefix bits_max(name) = ((1ULL << (bitssize)) - 1);

#define DEFINE_BITS(prefix, name, bitssize, pre_name) \
	prefix bits_offset(name) = next_offset(bits_offset(pre_name), bits_size(pre_name));\
	prefix bits_size(name) = (bitssize);\
	prefix bits_mask(name) = get_bits_mask(bits_offset(name), bits_size(name));\
	prefix name = bits_mask(name);\
	prefix bits_min(name) = encode_bits(name, 1); \
	prefix bits_max(name) = ((1ULL << ((bitssize) + bits_offset(name))) - 1);


