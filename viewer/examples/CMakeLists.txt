# ------------------------------------------------------------------------------------
# Project: CellForge
# Copyright (C) 2026, Melina Aero Teknoloji Gelistirme ve Dizayn Burosu A.S., Istanbul
# Author: Onur Tuncer, PhD
#
# SPDX-License-Identifier: LGPL-2.1-only
# License-Filename: LICENSE
# ------------------------------------------------------------------------------------

cmake_minimum_required(VERSION 3.18)
project(cellforge_viewer_examples LANGUAGES CXX)

find_package(Qt6 REQUIRED COMPONENTS Widgets)
find_package(OpenCASCADE REQUIRED)
set(CMAKE_AUTOMOC ON)

add_executable(cellforge_viewer_example main.cpp)

target_compile_features(cellforge_viewer_example PRIVATE cxx_std_20)

# Base.h requires one of these to be defined — detect from CMake platform vars.
if(WIN32)
  target_compile_definitions(cellforge_viewer_example PRIVATE CF_PLATFORM_WINDOWS)
elseif(UNIX)
  target_compile_definitions(cellforge_viewer_example PRIVATE CF_PLATFORM_LINUX)
endif()

target_link_libraries(
  cellforge_viewer_example
  PRIVATE CellForge::platform_qt
          CellForge::viewer
          CellForge::core
          Qt6::Widgets
          TKPrim # BRepPrimAPI_Make*
)

set_target_properties(cellforge_viewer_example PROPERTIES OUTPUT_NAME CellForgeViewerExample)

# ─── Runtime deployment (Windows) ─────────────────────────────────────────────
if(WIN32)
  get_target_property(_qt6_dll Qt6::Core IMPORTED_LOCATION_RELEASE)
  get_filename_component(_qt6_bin "${_qt6_dll}" DIRECTORY)
  get_filename_component(_qt6_root "${_qt6_bin}" DIRECTORY)

  file(GENERATE OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/qt.conf" CONTENT "[Paths]\nPlugins = ${_qt6_root}/Qt6/plugins\n")
  add_custom_command(
    TARGET cellforge_viewer_example
    POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different "${CMAKE_CURRENT_BINARY_DIR}/qt.conf"
            "$<TARGET_FILE_DIR:cellforge_viewer_example>/qt.conf"
    COMMENT "Deploying qt.conf")

  add_custom_command(
    TARGET cellforge_viewer_example
    POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different "$<TARGET_FILE:cellforge_core>" "$<TARGET_FILE:viewer>"
            "$<TARGET_FILE:cellforge_platform_qt>" "$<TARGET_FILE_DIR:cellforge_viewer_example>"
    COMMENT "Deploying CellForge DLLs")

  file(GLOB _vcpkg_bin_dlls "${_qt6_bin}/*.dll")
  add_custom_command(
    TARGET cellforge_viewer_example
    POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different ${_vcpkg_bin_dlls} "$<TARGET_FILE_DIR:cellforge_viewer_example>"
    COMMENT "Deploying vcpkg runtime DLLs")
endif()

if(WIN32)
  add_subdirectory(mfc)
endif()
