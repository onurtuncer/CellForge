// ------------------------------------------------------------------------------------
// Project: CellForge
// Copyright (C) 2026, Melina Aero Teknoloji Gelistirme ve Dizayn Burosu A.S., Istanbul
// Author: Onur Tuncer, PhD
//
// SPDX-License-Identifier: LGPL-2.1-only
// License-Filename: LICENSE
// ------------------------------------------------------------------------------------

#pragma once

#ifdef CF_PLATFORM_WINDOWS
#include <Windows.h>
#endif

#include <algorithm>
#include <array>
#include <cmath>
#include <cstddef>
#include <cstdlib>
#include <cstdarg>
#include <filesystem>
#include <fstream>
#include <functional>
#include <limits>
#include <map>
#include <memory>
#include <mutex>
#include <random>
#include <set>
#include <string>
#include <string_view>
#include <type_traits>
#include <unordered_map>
#include <utility>
#include <vector>
#include <filesystem>
#include <thread>

#include <CellForge/Version.h>
#include <CellForge/Assert.h>
#include <CellForge/Base.h>
#include <CellForge/Event/Event.h>
#include <CellForge/Log.h>
#include <CellForge/Memory.h>