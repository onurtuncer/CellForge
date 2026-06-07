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
option(CELLFORGE_ROBOT_BUILD_TESTS "Build robot unit tests" OFF)

# ─── Dependencies ─────────────────────────────────────────────────────────────
find_package(Eigen3 REQUIRED)
find_package(pinocchio REQUIRED)
find_package(tesseract_environment REQUIRED)
find_package(tesseract_common REQUIRED)

# ─── Library ──────────────────────────────────────────────────────────────────
add_library(robot SHARED src/RobotModel.cpp)
add_library(CellForge::robot ALIAS robot)
set_target_properties(robot PROPERTIES OUTPUT_NAME cellforge_robot)

target_cxx_version(robot PUBLIC VERSION ${CELLFORGE_CXX_VERSION})
target_compile_options(robot PRIVATE ${CELLFORGE_COMPILE_OPTIONS_PRIVATE})
target_compile_options(robot PUBLIC ${CELLFORGE_COMPILE_OPTIONS_PUBLIC})
target_compile_definitions(robot PUBLIC ${CELLFORGE_COMPILE_DEFINITIONS})

target_include_directories(
  robot PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include> $<INSTALL_INTERFACE:include>
  PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include/CellForge)

target_link_libraries(
  robot
  PUBLIC Eigen3::Eigen
         pinocchio::pinocchio
         tesseract::tesseract_environment
         tesseract::tesseract_common)

# ─── Install & CMake package export ───────────────────────────────────────────
include(CMakePackageConfigHelpers)
include(GNUInstallDirs)

install(
  TARGETS robot
  EXPORT CellForge-robot-targets
  RUNTIME_DEPENDENCY_SET CF_runtime_deps
  RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}"
  LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}"
  ARCHIVE DESTINATION "${CMAKE_INSTALL_LIBDIR}")
install(
  EXPORT CellForge-robot-targets
  NAMESPACE CellForge::
  DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/CellForge"
  FILE robot-targets.cmake)

file(
  WRITE "${CMAKE_CURRENT_BINARY_DIR}/robot-config.cmake.in"
  "@PACKAGE_INIT@
include(CMakeFindDependencyMacro)
find_dependency(Eigen3              REQUIRED)
find_dependency(pinocchio           REQUIRED)
find_dependency(tesseract_environment REQUIRED)
find_dependency(tesseract_common    REQUIRED)
if(NOT TARGET CellForge::robot)
  include(\"\${CMAKE_CURRENT_LIST_DIR}/robot-targets.cmake\")
endif()
")
configure_package_config_file(
  "${CMAKE_CURRENT_BINARY_DIR}/robot-config.cmake.in" "${CMAKE_CURRENT_BINARY_DIR}/robot-config.cmake"
  INSTALL_DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/CellForge" NO_CHECK_REQUIRED_COMPONENTS_MACRO)
install(FILES "${CMAKE_CURRENT_BINARY_DIR}/robot-config.cmake" DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/CellForge")

install(
  DIRECTORY include/CellForge
  DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}"
  FILES_MATCHING
  PATTERN "*.h"
  PATTERN "*.hpp")

# Register with the root umbrella
set(SUPPORTED_COMPONENTS ${SUPPORTED_COMPONENTS} "robot" PARENT_SCOPE)

if(CELLFORGE_ROBOT_BUILD_TESTS)
  add_subdirectory(test)
endif()
