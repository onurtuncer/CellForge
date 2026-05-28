# ------------------------------------------------------------------------------------
# Project: CellForge
# Copyright (C) 2026, Melina Aero Teknoloji Gelistirme ve Dizayn Burosu A.S., Istanbul
# Author: Onur Tuncer, PhD
#
# SPDX-License-Identifier: LGPL-2.1-only
# License-Filename: LICENSE
# ------------------------------------------------------------------------------------

cmake_minimum_required(VERSION 3.18)
project(cellforge_viewer_mfc_example LANGUAGES CXX)

# Cancel CMAKE_AUTOMOC inherited from the parent gui/examples scope — no Qt here.
set(CMAKE_AUTOMOC OFF)

find_package(OpenCASCADE REQUIRED)

# UseOfMfc=Dynamic must be set at directory scope so the VS generator applies it
# to the target; VS_GLOBAL_UseOfMfc is the per-target override.
set(CMAKE_MFC_FLAG 2)

add_executable(cellforge_viewer_mfc_example main.cpp)

target_compile_features(cellforge_viewer_mfc_example PRIVATE cxx_std_20)

target_compile_definitions(cellforge_viewer_mfc_example PRIVATE
    CF_PLATFORM_WINDOWS
    _AFXDLL
    UNICODE
    _UNICODE
)

set_property(TARGET cellforge_viewer_mfc_example PROPERTY VS_GLOBAL_UseOfMfc "2")

target_link_libraries(cellforge_viewer_mfc_example
    PRIVATE
        CellForge::platform_mfc
        CellForge::viewer
        CellForge::core
        TKPrim          # BRepPrimAPI_Make*
)

set_target_properties(cellforge_viewer_mfc_example PROPERTIES
    OUTPUT_NAME CellForgeViewerMfcExample
)

# ─── Runtime deployment (Windows) ─────────────────────────────────────────────
# Locate the vcpkg bin directory from yaml-cpp so we can glob all runtime DLLs
# (covers OCCT, Qt bootstrap, spdlog, flecs, Tracy, yaml-cpp in one pass).
find_package(yaml-cpp CONFIG REQUIRED)
get_target_property(_yamlcpp_dll yaml-cpp::yaml-cpp IMPORTED_LOCATION_RELEASE)
get_filename_component(_vcpkg_bin "${_yamlcpp_dll}" DIRECTORY)

add_custom_command(TARGET cellforge_viewer_mfc_example POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "$<TARGET_FILE:cellforge_core>"
        "$<TARGET_FILE:viewer>"
        "$<TARGET_FILE:cellforge_platform_mfc>"
        "$<TARGET_FILE_DIR:cellforge_viewer_mfc_example>"
    COMMENT "Deploying CellForge DLLs alongside MFC viewer example"
)

file(GLOB _vcpkg_bin_dlls "${_vcpkg_bin}/*.dll")
add_custom_command(TARGET cellforge_viewer_mfc_example POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${_vcpkg_bin_dlls}
        "$<TARGET_FILE_DIR:cellforge_viewer_mfc_example>"
    COMMENT "Deploying vcpkg runtime DLLs"
)
