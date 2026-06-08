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
option(CELLFORGE_WORKCELL_BUILD_TESTS "Build workcell unit tests" OFF)

# ─── Dependencies ─────────────────────────────────────────────────────────────
find_package(flecs CONFIG REQUIRED)
find_package(OpenCASCADE REQUIRED)

# ─── Library ──────────────────────────────────────────────────────────────────
add_library(workcell SHARED src/WorldRegistry.cpp src/SceneLoader.cpp src/SceneQuery.cpp)
add_library(CellForge::workcell ALIAS workcell)
set_target_properties(workcell PROPERTIES OUTPUT_NAME cellforge_workcell)

target_cxx_version(workcell PUBLIC VERSION ${CELLFORGE_CXX_VERSION})
target_compile_options(workcell PRIVATE ${CELLFORGE_COMPILE_OPTIONS_PRIVATE})
target_compile_options(workcell PUBLIC ${CELLFORGE_COMPILE_OPTIONS_PUBLIC})
target_compile_definitions(workcell PUBLIC ${CELLFORGE_COMPILE_DEFINITIONS})

target_include_directories(
  workcell PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include> $<INSTALL_INTERFACE:include>
                  $<BUILD_INTERFACE:${OpenCASCADE_INCLUDE_DIR}> PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include/CellForge)

target_link_libraries(
  workcell PUBLIC flecs::flecs TKernel # Standard_* types required by gp_*
                  TKMath # gp_Trsf, gp_Dir, gp_Pnt, gp_Vec
)

# ─── Install & CMake package export ───────────────────────────────────────────
include(CMakePackageConfigHelpers)
include(GNUInstallDirs)

install(
  TARGETS workcell
  EXPORT CellForge-workcell-targets
  RUNTIME_DEPENDENCY_SET CF_runtime_deps
  RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}"
  LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}"
  ARCHIVE DESTINATION "${CMAKE_INSTALL_LIBDIR}")
install(
  EXPORT CellForge-workcell-targets
  NAMESPACE CellForge::
  DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/CellForge"
  FILE workcell-targets.cmake)

file(
  WRITE "${CMAKE_CURRENT_BINARY_DIR}/workcell-config.cmake.in"
  "@PACKAGE_INIT@
include(CMakeFindDependencyMacro)
find_dependency(flecs       CONFIG REQUIRED)
find_dependency(OpenCASCADE        REQUIRED)
if(NOT TARGET CellForge::workcell)
  include(\"\${CMAKE_CURRENT_LIST_DIR}/workcell-targets.cmake\")
endif()
")
configure_package_config_file(
  "${CMAKE_CURRENT_BINARY_DIR}/workcell-config.cmake.in" "${CMAKE_CURRENT_BINARY_DIR}/workcell-config.cmake"
  INSTALL_DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/CellForge" NO_CHECK_REQUIRED_COMPONENTS_MACRO)
install(FILES "${CMAKE_CURRENT_BINARY_DIR}/workcell-config.cmake" DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/CellForge")

install(
  DIRECTORY include/CellForge
  DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}"
  FILES_MATCHING
  PATTERN "*.h")

# Register with the root umbrella
set(SUPPORTED_COMPONENTS ${SUPPORTED_COMPONENTS} "workcell" PARENT_SCOPE)

if(CELLFORGE_WORKCELL_BUILD_TESTS)
  add_subdirectory(test)
endif()
