# ------------------------------------------------------------------------------------
# Project: CellForge
# Copyright (C) 2026, Melina Aero Teknoloji Gelistirme ve Dizayn Burosu A.S., Istanbul
# Author: Onur Tuncer, PhD
#
# SPDX-License-Identifier: LGPL-2.1-only
# License-Filename: LICENSE
# ------------------------------------------------------------------------------------

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
option(CELLFORGE_PROJECT_BUILD_TESTS "Build project unit tests" OFF)

# ─── Library ──────────────────────────────────────────────────────────────────
add_library(project SHARED src/Project.cpp)
add_library(CellForge::project ALIAS project)
set_target_properties(project PROPERTIES OUTPUT_NAME cellforge_project)

target_cxx_version(project PUBLIC VERSION ${CELLFORGE_CXX_VERSION})
target_compile_options(project PRIVATE ${CELLFORGE_COMPILE_OPTIONS_PRIVATE})
target_compile_options(project PUBLIC ${CELLFORGE_COMPILE_OPTIONS_PUBLIC})
target_compile_definitions(project PUBLIC ${CELLFORGE_COMPILE_DEFINITIONS})

target_include_directories(project PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
                                          $<INSTALL_INTERFACE:include>)

target_link_libraries(
  project PUBLIC CellForge::core # IPersistenceBackend, Mathematics.h (Transform, NodeId, DocumentId)
)

# ─── Install & CMake package export ───────────────────────────────────────────
include(CMakePackageConfigHelpers)
include(GNUInstallDirs)

install(
  TARGETS project
  EXPORT CellForge-project-targets
  RUNTIME_DEPENDENCY_SET CF_runtime_deps
  RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}"
  LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}"
  ARCHIVE DESTINATION "${CMAKE_INSTALL_LIBDIR}")
install(
  EXPORT CellForge-project-targets
  NAMESPACE CellForge::
  DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/CellForge"
  FILE project-targets.cmake)

file(
  WRITE "${CMAKE_CURRENT_BINARY_DIR}/project-config.cmake.in"
  "@PACKAGE_INIT@
include(CMakeFindDependencyMacro)
if(NOT TARGET CellForge::project)
  include(\"\${CMAKE_CURRENT_LIST_DIR}/project-targets.cmake\")
endif()
")
configure_package_config_file(
  "${CMAKE_CURRENT_BINARY_DIR}/project-config.cmake.in" "${CMAKE_CURRENT_BINARY_DIR}/project-config.cmake"
  INSTALL_DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/CellForge" NO_CHECK_REQUIRED_COMPONENTS_MACRO)
install(FILES "${CMAKE_CURRENT_BINARY_DIR}/project-config.cmake" DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/CellForge")

install(
  DIRECTORY include/CellForge
  DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}"
  FILES_MATCHING
  PATTERN "*.h")

# Register with the root umbrella
set(SUPPORTED_COMPONENTS ${SUPPORTED_COMPONENTS} "project" PARENT_SCOPE)

if(CELLFORGE_PROJECT_BUILD_TESTS)
  add_subdirectory(test)
endif()
