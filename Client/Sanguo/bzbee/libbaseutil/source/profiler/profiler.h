/*! bzb engine
 * file profiler.h
 * author xkj
 * date 2019/09/29
 */
#pragma once

#include "tracy/Tracy.hpp"
#include "tracy/common/TracySystem.hpp"

#define PRO_WHITE
#define PRO_GREEN

enum ProfilerCategory
{
	kProfileDefault			= 1 << 0,
	kProfileRenderDevice	= 1 << 1,
	kProfileParticleSystem	= 1 << 2,
	kProfileAll				= 0xFFFF
};

extern int g_profileMask;
void enableProfileCategory(int masks);
void disableProfileCategory(int masks);

#if defined(TRACY_ENABLE)

#define PRO_THREAD(name) tracy::SetThreadName(name);
#define PRO_FRAME_MARK FrameMark
#if defined TRACY_HAS_CALLSTACK && defined TRACY_CALLSTACK
#define PRO_SECTION(name,category) static const tracy::SourceLocationData TracyConcat(__tracy_source_location,__LINE__) { #name, __FUNCTION__,  __FILE__, (uint32_t)__LINE__, 0 }; tracy::ScopedZone var##name ( &TracyConcat(__tracy_source_location,__LINE__), TRACY_CALLSTACK, g_profileMask & category );
#else
#define PRO_SECTION(name,category) static const tracy::SourceLocationData TracyConcat(__tracy_source_location, __LINE__) { #name, __FUNCTION__, __FILE__, (uint32_t)__LINE__, 0 }; tracy::ScopedZone var##name(&TracyConcat(__tracy_source_location, __LINE__), g_profileMask & category);
#endif

#define PRO_EVENT(name,category) if(g_profileMask & category) { TracyMessageL(name); }

#else
#define PRO_THREAD(name)
#define PRO_FRAME_MARK
#define PRO_SECTION(name,category)
#define PRO_EVENT(name,category)
#endif