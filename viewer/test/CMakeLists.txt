# ------------------------------------------------------------------------------------
# Project: CellForge
# Copyright (C) 2026, Melina Aero Teknoloji Gelistirme ve Dizayn Burosu A.S., Istanbul
# Author: Onur Tuncer, PhD
#
# SPDX-License-Identifier: LGPL-2.1-only
# License-Filename: LICENSE
# ------------------------------------------------------------------------------------

find_package(Catch2 3 REQUIRED)

include(CTest)
include(Catch)

# Add Catch2 test targets here as the viewer gains unit-testable logic. Example skeleton:
#
# add_executable(test_viewer_smoke test_viewer_smoke.cpp) target_link_libraries(test_viewer_smoke PRIVATE
# CellForge::viewer Catch2::Catch2WithMain) catch_discover_tests(test_viewer_smoke DISCOVERY_MODE PRE_TEST)
