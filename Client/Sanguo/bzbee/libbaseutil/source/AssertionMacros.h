/*!
* \file AssertionMacros.h
*
* \author mattho
* \date 2016/08/23
*
*
*/
#pragma once

#ifdef NDEBUG
	#include "LogUtil.h"
#endif

NS_BEGIN

NS_END

#undef ASSERT

#ifndef NDEBUG
	#define ASSERT(expr)		assert((expr))
	#define VERIFY(expr)		assert((expr))
#else
	#define ASSERT(expr)		
	#define VERIFY(expr)		if (!(expr)) {}
#endif

#undef ASSERT_LOG

#ifndef NDEBUG
	#define ASSERT_LOG(expr, format, ...)	assert((expr))
#else
	#define ASSERT_LOG(expr, format, ...)	
#endif

#define ENSURE_LOG(expr, format, ...)		(!!(expr))

#ifndef NDEBUG
	#define ASSERT_NOT_IMPLEMENTED				ASSERT(false)
	#define ASSERT_UNREACHABLE					ASSERT(false)
#else
	#define ASSERT_NOT_IMPLEMENTED				LOGE("Assertion failed: ASSERT_NOT_IMPLEMENTED")
	#define ASSERT_UNREACHABLE					LOGE("Assertion failed: ASSERT_UNREACHABLE")
#endif

#undef verify
#ifndef NDEBUG
#define verify( x )		( ( x ) ? true : ( ASSERT(x), false ) )
#else
#define verify( x )		( ( x ) ? true : false )
#endif

