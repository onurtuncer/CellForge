
#pragma once

#include "Base.h"
#include "Log.h"

#ifdef CF_PLATFORM_WINDOWS
	#define CF_DEBUG_BREAK __debugbreak()
#elif defined(CF_COMPILER_CLANG)
	#define CF_DEBUG_BREAK __builtin_debugtrap()
#else
	#define CF_DEBUG_BREAK
#endif

#ifdef CF_DEBUG
	#define CF_ENABLE_ASSERTS
#endif

#define CF_ENABLE_VERIFY

#ifdef CF_ENABLE_ASSERTS
	#ifdef CF_COMPILER_CLANG
		#define CF_CORE_ASSERT_MESSAGE_INTERNAL(...)  ::Hazel::Log::PrintAssertMessage(::Hazel::Log::Type::Core, "Assertion Failed", ##__VA_ARGS__)
		#define CF_ASSERT_MESSAGE_INTERNAL(...)  ::Hazel::Log::PrintAssertMessage(::Hazel::Log::Type::Client, "Assertion Failed", ##__VA_ARGS__)
	#else
		#define CF_CORE_ASSERT_MESSAGE_INTERNAL(...)  ::Hazel::Log::PrintAssertMessage(::Hazel::Log::Type::Core, "Assertion Failed" __VA_OPT__(,) __VA_ARGS__)
		#define CF_ASSERT_MESSAGE_INTERNAL(...)  ::Hazel::Log::PrintAssertMessage(::Hazel::Log::Type::Client, "Assertion Failed" __VA_OPT__(,) __VA_ARGS__)
	#endif

	#define CF_CORE_ASSERT(condition, ...) { if(!(condition)) { CF_CORE_ASSERT_MESSAGE_INTERNAL(__VA_ARGS__); CF_DEBUG_BREAK; } }
	#define CF_ASSERT(condition, ...) { if(!(condition)) { CF_ASSERT_MESSAGE_INTERNAL(__VA_ARGS__); CF_DEBUG_BREAK; } }
#else
	#define CF_CORE_ASSERT(condition, ...)
	#define CF_ASSERT(condition, ...)
#endif

#ifdef CF_ENABLE_VERIFY
	#ifdef CF_COMPILER_CLANG
		#define CF_CORE_VERIFY_MESSAGE_INTERNAL(...)  ::Hazel::Log::PrintAssertMessage(::Hazel::Log::Type::Core, "Verify Failed", ##__VA_ARGS__)
		#define CF_VERIFY_MESSAGE_INTERNAL(...)  ::Hazel::Log::PrintAssertMessage(::Hazel::Log::Type::Client, "Verify Failed", ##__VA_ARGS__)
	#else
		#define CF_CORE_VERIFY_MESSAGE_INTERNAL(...)  ::Hazel::Log::PrintAssertMessage(::Hazel::Log::Type::Core, "Verify Failed" __VA_OPT__(,) __VA_ARGS__)
		#define CF_VERIFY_MESSAGE_INTERNAL(...)  ::Hazel::Log::PrintAssertMessage(::Hazel::Log::Type::Client, "Verify Failed" __VA_OPT__(,) __VA_ARGS__)
	#endif

	#define CF_CORE_VERIFY(condition, ...) { if(!(condition)) { CF_CORE_VERIFY_MESSAGE_INTERNAL(__VA_ARGS__); CF_DEBUG_BREAK; } }
	#define CF_VERIFY(condition, ...) { if(!(condition)) { CF_VERIFY_MESSAGE_INTERNAL(__VA_ARGS__); CF_DEBUG_BREAK; } }
#else
	#define CF_CORE_VERIFY(condition, ...)
	#define CF_VERIFY(condition, ...)
#endif