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

project(cellforge_core VERSION 0.1.0 LANGUAGES CXX)

if(WIN32)
  set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
endif()

include(${CMAKE_CURRENT_SOURCE_DIR}/../cmake/cellforge_macros.cmake)
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../cmake/")
cellforge_variables()

# ─── Dependencies ─────────────────────────────────────────────────────────────
find_package(spdlog   CONFIG REQUIRED)
find_package(yaml-cpp CONFIG REQUIRED)
find_package(flecs    CONFIG REQUIRED)
find_package(Tracy    CONFIG REQUIRED)

# ─── Library ─────────────────────────────────────────────────────────────────
add_library(cellforge_core SHARED
    src/Application.cpp
    src/ApplicationSettings.cpp
    src/Log.cpp
    src/Ref.cpp
    src/PreCompiledHeader.cpp
    src/ecs/UUID.cpp
    # Memory.cpp excluded: Allocator tracking is only active when CF_TRACK_MEMORY
    # is defined. When it is not set, hnew/hdelete expand to new/delete and the
    # Allocator methods are never called, so no implementation is needed.
)
add_library(CellForge::core ALIAS cellforge_core)
set_target_properties(cellforge_core PROPERTIES OUTPUT_NAME cellforge_core)

target_cxx_version(cellforge_core PUBLIC VERSION ${CELLFORGE_CXX_VERSION})
target_compile_options(cellforge_core PRIVATE ${CELLFORGE_COMPILE_OPTIONS_PRIVATE})
target_compile_options(cellforge_core PUBLIC  ${CELLFORGE_COMPILE_OPTIONS_PUBLIC})
target_compile_definitions(cellforge_core PUBLIC ${CELLFORGE_COMPILE_DEFINITIONS})

# Base.h requires one of these to be defined — detect from CMake platform vars.
if(WIN32)
    target_compile_definitions(cellforge_core PUBLIC CF_PLATFORM_WINDOWS)
elseif(UNIX)
    target_compile_definitions(cellforge_core PUBLIC CF_PLATFORM_LINUX)
endif()

# Version.h requires exactly one of CF_DEBUG / CF_RELEASE / CF_DIST.
target_compile_definitions(cellforge_core PUBLIC
    $<$<CONFIG:Debug>:CF_DEBUG>
    $<$<CONFIG:Release>:CF_RELEASE>
    $<$<CONFIG:RelWithDebInfo>:CF_RELEASE>
    $<$<CONFIG:MinSizeRel>:CF_DIST>
)

target_include_directories(cellforge_core
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/include/CellForge
)

target_link_libraries(cellforge_core
    PUBLIC
        spdlog::spdlog
        flecs::flecs
        Tracy::TracyClient
    PRIVATE
        yaml-cpp::yaml-cpp
)

# ─── Install & CMake package export ───────────────────────────────────────────
include(CMakePackageConfigHelpers)
include(GNUInstallDirs)

install(TARGETS cellforge_core
    EXPORT   CellForge-core-targets
    RUNTIME  DESTINATION "${CMAKE_INSTALL_BINDIR}"
    LIBRARY  DESTINATION "${CMAKE_INSTALL_LIBDIR}"
    ARCHIVE  DESTINATION "${CMAKE_INSTALL_LIBDIR}"
)
install(EXPORT CellForge-core-targets
    NAMESPACE   CellForge::
    DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/CellForge"
    FILE        core-targets.cmake
)

file(WRITE "${CMAKE_CURRENT_BINARY_DIR}/core-config.cmake.in"
"@PACKAGE_INIT@
include(CMakeFindDependencyMacro)
find_dependency(spdlog   CONFIG REQUIRED)
find_dependency(yaml-cpp CONFIG REQUIRED)
find_dependency(flecs    CONFIG REQUIRED)
if(NOT TARGET CellForge::core)
  include(\"\${CMAKE_CURRENT_LIST_DIR}/core-targets.cmake\")
endif()
")
configure_package_config_file(
    "${CMAKE_CURRENT_BINARY_DIR}/core-config.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/core-config.cmake"
    INSTALL_DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/CellForge"
    NO_CHECK_REQUIRED_COMPONENTS_MACRO
)
install(FILES "${CMAKE_CURRENT_BINARY_DIR}/core-config.cmake"
    DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/CellForge"
)

install(
    DIRECTORY   include/CellForge
    DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}"
    FILES_MATCHING PATTERN "*.h"
)

# Register with the root umbrella
set(SUPPORTED_COMPONENTS ${SUPPORTED_COMPONENTS} "core" PARENT_SCOPE)

# ─── Tests ───────────────────────────────────────────────────────────────────
option(CELLFORGE_CORE_BUILD_TESTS "Build core unit tests" OFF)

if(CELLFORGE_CORE_BUILD_TESTS)
    add_subdirectory(test)
endif()
