# ------------------------------------------------------------------------------------
# Project: CellForge
# Copyright (C) 2026, Melina Aero Teknoloji Gelistirme ve Dizayn Burosu A.S., Istanbul
# Author: Onur Tuncer, PhD
#
# SPDX-License-Identifier: LGPL-2.1-only
# License-Filename: LICENSE
# ------------------------------------------------------------------------------------

cmake_minimum_required(VERSION 3.18)
project(cellforge_platform_mfc_examples LANGUAGES CXX)

add_executable(example_mfc_event_log main.cpp)

target_compile_features(example_mfc_event_log PRIVATE cxx_std_20)

target_compile_definitions(
  example_mfc_event_log PRIVATE CF_PLATFORM_WINDOWS _AFXDLL UNICODE _UNICODE WIN32_LEAN_AND_MEAN)

# VS project must be set to "Use MFC in a Shared DLL" or the linker won't find
# the MFC entry-point symbol that replaces the standard CRT one.
set_property(TARGET example_mfc_event_log PROPERTY VS_GLOBAL_UseOfMfc "2")

target_link_libraries(example_mfc_event_log PRIVATE CellForge::platform_mfc CellForge::core)

# ─── Runtime deployment (Windows) ─────────────────────────────────────────────
# Locate vcpkg bin dir from the yaml-cpp imported target so we can deploy all
# runtime DLLs that cellforge_core depends on (yaml-cpp, spdlog, flecs, Tracy).
find_package(yaml-cpp CONFIG REQUIRED)
find_package(spdlog CONFIG REQUIRED)
find_package(flecs CONFIG REQUIRED)
find_package(Tracy CONFIG REQUIRED)

get_target_property(_yamlcpp_dll yaml-cpp::yaml-cpp IMPORTED_LOCATION_RELEASE)
get_filename_component(_vcpkg_bin "${_yamlcpp_dll}" DIRECTORY)

add_custom_command(
  TARGET example_mfc_event_log
  POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy_if_different "$<TARGET_FILE:cellforge_core>"
          "$<TARGET_FILE:cellforge_platform_mfc>" "${_vcpkg_bin}/yaml-cpp.dll"
          "${_vcpkg_bin}/spdlog.dll" "${_vcpkg_bin}/flecs.dll" "${_vcpkg_bin}/TracyClient.dll"
          "$<TARGET_FILE_DIR:example_mfc_event_log>"
  COMMENT "Deploying CellForge and vcpkg runtime DLLs alongside MFC example")
