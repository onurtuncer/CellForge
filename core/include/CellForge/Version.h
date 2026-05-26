// ------------------------------------------------------------------------------------
// Project: CellForge
// Copyright (C) 2026, Melina Aero Teknoloji Gelistirme ve Dizayn Burosu A.S., Istanbul
// Author: Onur Tuncer, PhD
//
// SPDX-License-Identifier: LGPL-2.1-only
// License-Filename: LICENSE
// ------------------------------------------------------------------------------------

#pragma once

#define CF_VERSION "2026.0.0"

//
// Build Configuration
//
#if defined(CF_DEBUG)
	#define CF_BUILD_CONFIG_NAME "Debug"
#elif defined(CF_RELEASE)
	#define CF_BUILD_CONFIG_NAME "Release"
#elif defined(CF_DIST)
	#define CF_BUILD_CONFIG_NAME "Dist"
#else
	#error Undefined configuration?
#endif

//
// Build Platform
//
#if defined(CF_PLATFORM_WINDOWS)
	#define CF_BUILD_PLATFORM_NAME "Windows x64"
#elif defined(HZ_PLATFORM_LINUX)
	#define CF_BUILD_PLATFORM_NAME "Linux"
#else
	#define CF_BUILD_PLATFORM_NAME "Unknown"
#endif

#define CF_VERSION_LONG "CellForge " CF_VERSION " (" CF_BUILD_PLATFORM_NAME " " CF_BUILD_CONFIG_NAME ")"