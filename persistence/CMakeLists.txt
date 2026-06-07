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
option(CELLFORGE_PERSISTENCE_BUILD_TESTS "Build persistence unit tests" OFF)

# ─── Dependencies ─────────────────────────────────────────────────────────────
find_package(OpenCASCADE REQUIRED)

# ─── Library ──────────────────────────────────────────────────────────────────
add_library(persistence SHARED src/OcafPersistenceBackend.cpp)
add_library(CellForge::persistence ALIAS persistence)
set_target_properties(persistence PROPERTIES OUTPUT_NAME cellforge_persistence)

target_cxx_version(persistence PUBLIC VERSION ${CELLFORGE_CXX_VERSION})
target_compile_options(persistence PRIVATE ${CELLFORGE_COMPILE_OPTIONS_PRIVATE})
target_compile_options(persistence PUBLIC ${CELLFORGE_COMPILE_OPTIONS_PUBLIC})
target_compile_definitions(persistence PUBLIC ${CELLFORGE_COMPILE_DEFINITIONS})

target_include_directories(
  persistence
  PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include> $<INSTALL_INTERFACE:include>
  PRIVATE # OCCT headers are private — consumers never see them through this library.
          ${OpenCASCADE_INCLUDE_DIR}
          ${CMAKE_CURRENT_SOURCE_DIR}/include/CellForge)

target_link_libraries(
  persistence
  PUBLIC CellForge::core # IPersistenceBackend, IGeometryBackend (ShapeId), Mathematics.h
  PRIVATE TKernel # Standard_*, Handle, TCollection_*
          TKCAF # TDF_*, TDocStd_*
          TKLCAF # TDataStd_*, TNaming_*
          TKBin # BinDrivers::DefineFormat (pulls in TKBinL transitively)
)

# ─── Install & CMake package export ───────────────────────────────────────────
include(CMakePackageConfigHelpers)
include(GNUInstallDirs)

install(
  TARGETS persistence
  EXPORT CellForge-persistence-targets
  RUNTIME_DEPENDENCY_SET CF_runtime_deps
  RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}"
  LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}"
  ARCHIVE DESTINATION "${CMAKE_INSTALL_LIBDIR}")
install(
  EXPORT CellForge-persistence-targets
  NAMESPACE CellForge::
  DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/CellForge"
  FILE persistence-targets.cmake)

file(
  WRITE "${CMAKE_CURRENT_BINARY_DIR}/persistence-config.cmake.in"
  "@PACKAGE_INIT@
include(CMakeFindDependencyMacro)
find_dependency(OpenCASCADE REQUIRED)
if(NOT TARGET CellForge::persistence)
  include(\"\${CMAKE_CURRENT_LIST_DIR}/persistence-targets.cmake\")
endif()
")
configure_package_config_file(
  "${CMAKE_CURRENT_BINARY_DIR}/persistence-config.cmake.in" "${CMAKE_CURRENT_BINARY_DIR}/persistence-config.cmake"
  INSTALL_DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/CellForge" NO_CHECK_REQUIRED_COMPONENTS_MACRO)
install(FILES "${CMAKE_CURRENT_BINARY_DIR}/persistence-config.cmake"
        DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/CellForge")

install(
  DIRECTORY include/CellForge
  DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}"
  FILES_MATCHING
  PATTERN "*.h")

# Register with the root umbrella
set(SUPPORTED_COMPONENTS ${SUPPORTED_COMPONENTS} "persistence" PARENT_SCOPE)

if(CELLFORGE_PERSISTENCE_BUILD_TESTS)
  add_subdirectory(test)
endif()
