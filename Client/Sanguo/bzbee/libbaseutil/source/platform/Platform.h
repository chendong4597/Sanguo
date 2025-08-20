/*!
 * \file PlatformPrivate.h
 *
 * \author mattho
 * \date 2017/01/21
 *
 *
 */
#pragma once
#include "../CompileSpec.h"
#include "../BaseType.h"
#include "../EnumUtil.h"
#include "../AssertionMacros.h"


 //////////////////////////////////////////////////////////////////////////
#if PLATFORM_WIN32
	#define PLATFORM_HAS_BSD_TIME		0
	#define PLATFORM_USE_PTHREADS		0
	#include "win32/Win32SystemIncludes.h"

#elif PLATFORM_ANDROID
	#define PLATFORM_HAS_BSD_TIME		1
	#define PLATFORM_USE_PTHREADS		1
	#define PLATFORM_USES_ES2			1
	#include "android/AndroidSystemIncludes.h"

#elif PLATFORM_IOS
	#define PLATFORM_HAS_BSD_TIME		1
	#define PLATFORM_USE_PTHREADS		1
	#define PLATFORM_USES_ES2			1

#elif PLATFORM_HTML5
	#if defined(_MSC_VER)
	#define PLATFORM_HTML5_WIN32		1
	#else
	#define PLATFORM_HTML5_WIN32		0
	#endif

	#if PLATFORM_HTML5_WIN32
	#define PLATFORM_HAS_BSD_TIME		0
	#else
	#define PLATFORM_HAS_BSD_TIME		1
	#endif

	#define PLATFORM_USE_PTHREADS		0
	#include "emscripten/EmscriptenSystemIncludes.h"

#endif
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
#ifndef PLATFORM_HAS_BSD_TIME
	#define PLATFORM_HAS_BSD_TIME	1
#endif

#ifndef PLATFORM_USE_PTHREADS
	#define PLATFORM_USE_PTHREADS	1
#endif

#ifndef PLATFORM_USES_ES2
	#define PLATFORM_USES_ES2		0
#endif
//////////////////////////////////////////////////////////////////////////
