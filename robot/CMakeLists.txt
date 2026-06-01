# -------------------------------------------------------------------------------------
# Project: CellForge 
# Copyright (C) 2026, Melina Aero Teknoloji Gelistirme ve Dizayn Burosu A.S., Istanbul 
# Author: Onur Tuncer, PhD
#
# SPDX-License-Identifier: LGPL-2.1-only 
# License-Filename: LICENSE
# -------------------------------------------------------------------------------------

cmake_minimum_required(VERSION 3.18)

find_package(ros_industrial_cmake_boilerplate REQUIRED)
extract_package_metadata(pkg)
project(${pkg_extracted_name} VERSION ${pkg_extracted_version} LANGUAGES CXX)

if(WIN32)
  set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
endif()

include(${CMAKE_CURRENT_SOURCE_DIR}/../cmake/cellforge_macros.cmake)
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../cmake/")
cellforge_variables()

# ─── Options ──────────────────────────────────────────────────────────────────
option(CELLFORGE_WORKCELL_BUILD_TESTS "Build robot unit tests" OFF)