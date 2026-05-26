// ------------------------------------------------------------------------------------
// Project: CellForge
// Copyright (C) 2026, Melina Aero Teknoloji Gelistirme ve Dizayn Burosu A.S., Istanbul
// Author: Onur Tuncer, PhD
//
// SPDX-License-Identifier: LGPL-2.1-only
// License-Filename: LICENSE
// ------------------------------------------------------------------------------------

#pragma once

#include "Ref.h"

#include <functional>
#include  <memory>

// DLL export/import for static data members (CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS
// covers functions but not static class data — those need explicit decoration).
#if defined(CF_PLATFORM_WINDOWS)
  #if defined(cellforge_core_EXPORTS)
    #define CF_API __declspec(dllexport)
  #else
    #define CF_API __declspec(dllimport)
  #endif
#else
  #define CF_API __attribute__((visibility("default")))
#endif

namespace CellForge {

	void InitializeCore();
	void ShutdownCore();

}

#if !defined(CF_PLATFORM_WINDOWS) && !defined(CF_PLATFORM_LINUX)
	#error Unknown platform.
#endif

#define BIT(x) (1u << x)

#if defined(__GNUC__)
	#if defined(__clang__)
		#define CF_COMPILER_CLANG
	#else
		#define CF_COMPILER_GCC
	#endif
#elif defined(_MSC_VER)
	#define CF_COMPILER_MSVC
#endif

#define CF_BIND_EVENT_FN(fn) std::bind(&fn, this, std::placeholders::_1)

#ifdef CF_COMPILER_MSVC
	#define CF_FORCE_INLINE __forceinline
	#define CF_EXPLICIT_STATIC static
#elif defined(__GNUC__)
	#define CF_FORCE_INLINE __attribute__((always_inline)) inline
	#define CF_EXPLICIT_STATIC
#else
	#define CF_FORCE_INLINE inline
	#define CF_EXPLICIT_STATIC
#endif

namespace CellForge {

	// Pointer wrappers
	template<typename T>
	using Scope = std::unique_ptr<T>;
	template<typename T, typename ... Args>
	constexpr Scope<T> CreateScope(Args&& ... args)
	{
		return std::make_unique<T>(std::forward<Args>(args)...);
	}

	using byte = uint8_t;

	/** A simple wrapper for std::atomic_flag to avoid confusing
		function names usage. The object owning it can still be
		default copyable, but the copied flag is going to be reset.
	*/
	struct AtomicFlag
	{
		CF_FORCE_INLINE void SetDirty() { flag.clear(); }
		CF_FORCE_INLINE bool CheckAndResetIfDirty() { return !flag.test_and_set(); }

		explicit AtomicFlag() noexcept { flag.test_and_set(); }
		AtomicFlag(const AtomicFlag&) noexcept {}
		AtomicFlag& operator=(const AtomicFlag&) noexcept { return *this; }
		AtomicFlag(AtomicFlag&&) noexcept {};
		AtomicFlag& operator=(AtomicFlag&&) noexcept { return *this; }

	private:
		std::atomic_flag flag;
	};

	struct Flag
	{
		CF_FORCE_INLINE void SetDirty() noexcept { flag = true; }
		CF_FORCE_INLINE bool CheckAndResetIfDirty() noexcept
		{
			if (flag)
				return !(flag = !flag);
			else
				return false;
		}

		CF_FORCE_INLINE bool IsDirty() const noexcept { return flag; }

	private:
		bool flag = false;
	};

}