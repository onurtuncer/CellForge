

#pragma once

#define CF_ENABLE_PROFILING !CF_DIST

#if CF_ENABLE_PROFILING 
#include <tracy/Tracy.hpp>
#endif

#if CF_ENABLE_PROFILING
#define CF_PROFILE_MARK_FRAME			FrameMark;
// NOTE(Peter): Use HZ_PROFILE_FUNC ONLY at the top of a function
//				Use HZ_PROFILE_SCOPE / HZ_PROFILE_SCOPE_DYNAMIC for an inner scope
#define CF_PROFILE_FUNC(...)			ZoneScoped##__VA_OPT__(N(__VA_ARGS__))
#define CF_PROFILE_SCOPE(...)			CF_PROFILE_FUNC(__VA_ARGS__)
#define CF_PROFILE_SCOPE_DYNAMIC(NAME)  ZoneScoped; ZoneName(NAME, strlen(NAME))
#define CF_PROFILE_THREAD(...)          tracy::SetThreadName(__VA_ARGS__)
#else
#define CF_PROFILE_MARK_FRAME
#define CF_PROFILE_FUNC(...)
#define CF_PROFILE_SCOPE(...)
#define CF_PROFILE_SCOPE_DYNAMIC(NAME)
#define CF_PROFILE_THREAD(...)
#endif