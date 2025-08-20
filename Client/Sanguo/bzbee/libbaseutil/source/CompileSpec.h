/*!
 * \file CompileSpec.h
 *
 * \author mattho
 * \date 2016/09/20
 *
 *
 */
#pragma once

//////////////////////////////////////////////////////////////////////////
// Generates GCC version like this:  xxyyzz (eg. 030401)
// xx: major version, yy: minor version, zz: patch level
#if defined(__GNUC__)
	#define _GCC_VER	((__GNUC__ * 10000) + (__GNUC_MINOR__ * 100) + __GNUC_PATCHLEVEL__)
#endif
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
#if defined(_WIN32)
	#define PLATFORM_WIN32		1
	#define PLATFORM_DESKTOP	1

	#if defined(_WIN64)
	#define PLATFORM_64BITS		1
	#else
	#define PLATFORM_64BITS		0
	#endif

#elif defined(__ANDROID__)
	#define PLATFORM_ANDROID	1
	#define PLATFORM_DESKTOP	0
	//#define PLATFORM_64BITS		0

#elif defined(__EMSCRIPTEN__)
	#define PLATFORM_HTML5		1
	#define PLATFORM_DESKTOP	0
	#define PLATFORM_64BITS		0

#elif defined(__APPLE__)
	#define PLATFORM_IOS		1
	#define PLATFORM_DESKTOP	0
	#define PLATFORM_64BITS		1
	#define PLATFORM_APPLE		1

#else
	#error Compile for undefined platform!
#endif
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
#if defined(_MSC_VER)
	#if PLATFORM_HTML5
		#define FORCEINLINE		_forceinline
	#else
		#define FORCEINLINE		__forceinline
	#endif
		#define FORCENOINLINE	__declspec(noinline)

#elif defined(_GCC_VER)
	#ifdef _DEBUG
		/** Easier to debug */
		#define FORCEINLINE		inline
	#else
		/* Force code to be inline */
		#define FORCEINLINE		inline __attribute__ ((always_inline))
	#endif
		#define FORCENOINLINE	__attribute__((noinline))
#endif

#ifndef FORCEINLINE
		#define FORCEINLINE		inline
#endif

#ifndef FORCENOINLINE
		#define FORCENOINLINE
#endif

#ifndef INLINE_CONSTEXPR
	#if PLATFORM_WIN32 && defined(_MSC_VER) && (_MSC_VER < 1900)
		#define INLINE_CONSTEXPR __forceinline
	#else
		#define INLINE_CONSTEXPR inline constexpr
	#endif
#endif
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
#ifdef _DEBUG
	#define FORCEINLINE_DEBUG	inline
#else
	#define FORCEINLINE_DEBUG	FORCEINLINE
#endif
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
#if defined(_WIN32)
	#if defined(__clang__)
		#define GCC_ALIGN(n)	__attribute__((aligned(n)))

	#if defined(_MSC_VER)
		// With -fms-extensions, Clang will accept either alignment attribute
		#define MSC_ALIGN(n)	__declspec(align(n))
	#endif
	#else
		#define MSC_ALIGN(n)	__declspec(align(n))
	#endif

#else
		#define GCC_ALIGN(n)	__attribute__((aligned(n)))
#endif

#ifndef GCC_ALIGN						
		#define GCC_ALIGN(n)
#endif
#ifndef MSC_ALIGN						
		#define MSC_ALIGN(n)
#endif
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
namespace HelperUtils
{
	template<class T, unsigned int N> unsigned char(&_arrayCounter(const T(&)[N]))[N];
}

#undef ARRAY_SIZE
#define ARRAY_SIZE(ar)			(sizeof(HelperUtils::_arrayCounter(ar))+0)

#define CSTR_LENGTH(chs)		(ARRAY_SIZE(chs)-1)
#define CSTR_PARAM(ar)			(ar), (ARRAY_SIZE(ar)-1)

#undef STRUCT_OFFSET

#if PLATFORM_WIN32
	#include <stddef.h>
	#define STRUCT_OFFSET(struc, member)	offsetof(struc, member)
#else
	#define STRUCT_OFFSET(struc, member)	__builtin_offsetof(struc, member)
#endif

#ifndef PLATFORM_CACHE_LINE_SIZE
	#define PLATFORM_CACHE_LINE_SIZE		128
#endif
//////////////////////////////////////////////////////////////////////////
