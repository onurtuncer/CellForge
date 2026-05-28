# ------------------------------------------------------------------------------------
# Project: CellForge
# Copyright (C) 2026, Melina Aero Teknoloji Gelistirme ve Dizayn Burosu A.S., Istanbul
# Author: Onur Tuncer, PhD
#
# SPDX-License-Identifier: LGPL-2.1-only
# License-Filename: LICENSE
# ------------------------------------------------------------------------------------

cmake_minimum_required(VERSION 3.18)

# ─── vcpkg toolchain (must precede project()) ────────────────────────────────
include(cmake/Vcpkg.cmake)

# ─── Staging prefix ───────────────────────────────────────────────────────────
# Defined here (before project()) so the RICB bootstrap below can use it.
# VendorDependencies.cmake reads this cache variable; setting it once is enough.
set(VENDOR_INSTALL_PREFIX "${CMAKE_BINARY_DIR}/vendor_install"
    CACHE PATH "Staging install prefix for vendored and first-party packages")
file(MAKE_DIRECTORY "${VENDOR_INSTALL_PREFIX}")

# Make installed packages findable for all subsequent find_package() calls,
# including those inside add_subdirectory'd components.
list(PREPEND CMAKE_PREFIX_PATH "${VENDOR_INSTALL_PREFIX}")

# ─── Bootstrap: ros_industrial_cmake_boilerplate ──────────────────────────────
# RICB supplies extract_package_metadata(), configure_component(), and
# configure_package() — all called before or shortly after project().
# It contains no compiled code, so the first-run build takes only a few seconds.
find_package(ros_industrial_cmake_boilerplate QUIET)
if(NOT ros_industrial_cmake_boilerplate_FOUND)
  message(STATUS "CellForge: bootstrapping ros_industrial_cmake_boilerplate (first configure only)…")
  execute_process(
    COMMAND "${CMAKE_COMMAND}"
      -S "${CMAKE_SOURCE_DIR}/vendor/ros_industrial_cmake_boilerplate"
      -B "${CMAKE_BINARY_DIR}/bootstrap/ricb"
      -G "${CMAKE_GENERATOR}"
      "-DCMAKE_INSTALL_PREFIX=${VENDOR_INSTALL_PREFIX}"
      -DCMAKE_BUILD_TYPE=Release
    RESULT_VARIABLE _ricb_cfg
  )
  if(_ricb_cfg)
    message(FATAL_ERROR "RICB configure failed (exit ${_ricb_cfg})")
  endif()
  execute_process(
    COMMAND "${CMAKE_COMMAND}"
      --build "${CMAKE_BINARY_DIR}/bootstrap/ricb"
      --target install --config Release
    RESULT_VARIABLE _ricb_build
  )
  if(_ricb_build)
    message(FATAL_ERROR "RICB install failed (exit ${_ricb_build})")
  endif()
  find_package(ros_industrial_cmake_boilerplate REQUIRED)
endif()

# ─── CellForge project ────────────────────────────────────────────────────────
extract_package_metadata(pkg)
project(${pkg_extracted_name} VERSION ${pkg_extracted_version} LANGUAGES C CXX)

if(WIN32)
  set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
endif()

include(cmake/cellforge_macros.cmake)
list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake/")
cellforge_variables()

# ─── Third-party vendor packages (ExternalProject — built at build time) ──────
include(cmake/VendorDependencies.cmake)

# ─── CellForge first-party components ────────────────────────────────────────
# Each add_subdirectory appends its component name to SUPPORTED_COMPONENTS
# via set(SUPPORTED_COMPONENTS ... PARENT_SCOPE), exactly like tesseract.
set(SUPPORTED_COMPONENTS "")

include(cmake/CellForgeComponents.cmake)

# ─── Umbrella package config ──────────────────────────────────────────────────
# Generates lib/cmake/CellForge/CellForge-config.cmake so consumers can:
#   find_package(CellForge REQUIRED COMPONENTS viewer)
#   target_link_libraries(myapp PRIVATE CellForge::viewer)
#
# configure_package() (from RICB) generates an umbrella config that includes
# ${CMAKE_CURRENT_LIST_DIR}/${component}-config.cmake for each component.
# Each component installs its own config to lib/cmake/CellForge/ so the
# relative include resolves correctly.
configure_package(COMPONENT CellForge SUPPORTED_COMPONENTS ${SUPPORTED_COMPONENTS})

# ─── Integration tests (ExternalProject — tests against installed tesseract) ──
include(cmake/IntegrationTests.cmake)
