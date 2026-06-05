# ------------------------------------------------------------------------------------
# Project: CellForge
# Copyright (C) 2026, Melina Aero Teknoloji Gelistirme ve Dizayn Burosu A.S., Istanbul
# Author: Onur Tuncer, PhD
#
# SPDX-License-Identifier: LGPL-2.1-only
# License-Filename: LICENSE
# ------------------------------------------------------------------------------------

cmake_minimum_required(VERSION 3.18)
project(CellForgeTests LANGUAGES CXX)

enable_testing()

find_package(Catch2 3 REQUIRED)
find_package(tesseract REQUIRED COMPONENTS common scene_graph)

include(CTest)
include(Catch)

add_executable(test_tesseract_smoke integration/test_tesseract_smoke.cpp)
target_link_libraries(test_tesseract_smoke PRIVATE Catch2::Catch2WithMain tesseract::common tesseract::scene_graph)
target_compile_features(test_tesseract_smoke PRIVATE cxx_std_17)

# DISCOVERY_MODE PRE_TEST defers enumeration to ctest run-time, avoiding the "DLLs not on PATH" problem that occurs when
# cmake tries to execute the binary at build time on Windows.
catch_discover_tests(test_tesseract_smoke DISCOVERY_MODE PRE_TEST)
