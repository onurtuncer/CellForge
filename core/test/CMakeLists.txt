# ------------------------------------------------------------------------------------
# Project: CellForge
# Copyright (C) 2026, Melina Aero Teknoloji Gelistirme ve Dizayn Burosu A.S., Istanbul
# Author: Onur Tuncer, PhD
#
# SPDX-License-Identifier: LGPL-2.1-only
# License-Filename: LICENSE
# ------------------------------------------------------------------------------------

cmake_minimum_required(VERSION 3.18)
project(cellforge_core_tests LANGUAGES CXX)

find_package(Catch2 3 REQUIRED)

include(CTest)
include(Catch)

# ─── Shared helper to register a core test executable ─────────────────────────
function(cellforge_core_test target source)
    add_executable(${target} ${source})
    target_compile_features(${target} PRIVATE cxx_std_20)
    # CF_RELEASE: satisfies Version.h; disables assert message boxes on Windows.
    target_compile_definitions(${target} PRIVATE CF_RELEASE)
    target_link_libraries(${target}
        PRIVATE
            Catch2::Catch2WithMain
            CellForge::core
    )
    catch_discover_tests(${target} DISCOVERY_MODE PRE_TEST)
endfunction()

# ─── Test executables ─────────────────────────────────────────────────────────
cellforge_core_test(test_core_log  test_log.cpp)
cellforge_core_test(test_core_uuid test_uuid.cpp)
cellforge_core_test(test_core_ref  test_ref.cpp)
