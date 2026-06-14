# ------------------------------------------------------------------------------------
# Project: CellForge
# Copyright (C) 2026, Melina Aero Teknoloji Gelistirme ve Dizayn Burosu A.S., Istanbul
# Author: Onur Tuncer, PhD
#
# SPDX-License-Identifier: LGPL-2.1-only
# License-Filename: LICENSE
# ------------------------------------------------------------------------------------

cmake_minimum_required(VERSION 3.21)

# ─── vcpkg toolchain (must precede project()) ────────────────────────────────
include(cmake/Vcpkg.cmake)

# ─── Staging prefix ───────────────────────────────────────────────────────────
# Defined here (before project()) so the RICB bootstrap below can use it. VendorDependencies.cmake reads this cache
# variable; setting it once is enough.
set(VENDOR_INSTALL_PREFIX "${CMAKE_BINARY_DIR}/vendor_install"
    CACHE PATH "Staging install prefix for vendored and first-party packages")
file(MAKE_DIRECTORY "${VENDOR_INSTALL_PREFIX}")

# Make installed packages findable for all subsequent find_package() calls, including those inside add_subdirectory'd
# components.
list(PREPEND CMAKE_PREFIX_PATH "${VENDOR_INSTALL_PREFIX}")

# ─── Bootstrap: ros_industrial_cmake_boilerplate ──────────────────────────────
# RICB supplies extract_package_metadata(), configure_component(), and configure_package() — all called before or
# shortly after project(). It contains no compiled code, so the first-run build takes only a few seconds.
find_package(ros_industrial_cmake_boilerplate QUIET)
if(NOT ros_industrial_cmake_boilerplate_FOUND)
  message(STATUS "CellForge: bootstrapping ros_industrial_cmake_boilerplate (first configure only)…")
  execute_process(
    COMMAND
      "${CMAKE_COMMAND}" -S "${CMAKE_SOURCE_DIR}/vendor/ros_industrial_cmake_boilerplate" -B
      "${CMAKE_BINARY_DIR}/bootstrap/ricb" -G "${CMAKE_GENERATOR}" "-DCMAKE_INSTALL_PREFIX=${VENDOR_INSTALL_PREFIX}"
      -DCMAKE_BUILD_TYPE=Release
    RESULT_VARIABLE _ricb_cfg)
  if(_ricb_cfg)
    message(FATAL_ERROR "RICB configure failed (exit ${_ricb_cfg})")
  endif()
  execute_process(COMMAND "${CMAKE_COMMAND}" --build "${CMAKE_BINARY_DIR}/bootstrap/ricb" --target install --config
                          Release RESULT_VARIABLE _ricb_build)
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
# Each add_subdirectory appends its component name to SUPPORTED_COMPONENTS via set(SUPPORTED_COMPONENTS ...
# PARENT_SCOPE), exactly like tesseract.
set(SUPPORTED_COMPONENTS "")

include(cmake/CellForgeComponents.cmake)

# ─── Runtime dependency bundling ──────────────────────────────────────────────
# Resolves the transitive DLL closure of every CellForge target and installs only the DLLs that are actually needed (not
# the full vcpkg/vendor bin dirs). The two DIRECTORIES entries tell the resolver where to look: • vcpkg bin — Qt6, OCCT,
# Boost, spdlog, yaml-cpp, flecs, Tracy, … • vendor_install bin — Tesseract, boost_plugin_loader, …
if(WIN32)
  # vcpkg sets VCPKG_INSTALLED_DIR in manifest mode; fall back to the conventional in-source location when invoking
  # component sub-builds.
  if(NOT DEFINED VCPKG_INSTALLED_DIR)
    set(VCPKG_INSTALLED_DIR "${CMAKE_SOURCE_DIR}/vcpkg_installed")
  endif()

  install(
    RUNTIME_DEPENDENCY_SET
    CF_runtime_deps
    PRE_EXCLUDE_REGEXES
    "api-ms-win-.*" # Windows API sets — virtual, never on disk
    "ext-ms-win-.*"
    POST_EXCLUDE_REGEXES
    ".*[Ss]ystem32[/\\].*\.dll" # Windows system DLLs
    ".*[Ss]ys[Ww][Oo][Ww]64[/\\].*\.dll"
    DIRECTORIES
    "${VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}/bin"
    "${VENDOR_INSTALL_PREFIX}/bin"
    # First-party CellForge component DLLs (e.g. cellforge_core.dll) are not copied into a
    # shared output directory, so other components' RUNTIME_DEPENDENCY_SET resolution needs
    # each component's own build directory to find them.
    "${CMAKE_BINARY_DIR}/core"
    "${CMAKE_BINARY_DIR}/workcell"
    "${CMAKE_BINARY_DIR}/viewer"
    "${CMAKE_BINARY_DIR}/platform"
    "${CMAKE_BINARY_DIR}/persistence"
    "${CMAKE_BINARY_DIR}/project"
    "${CMAKE_BINARY_DIR}/robot"
    DESTINATION
    "${CMAKE_INSTALL_BINDIR}")

  # Tesseract factory/plugin DLLs are discovered at runtime by boost_plugin_loader and are therefore invisible to the
  # static linker — install them explicitly.
  install(
    DIRECTORY "${VENDOR_INSTALL_PREFIX}/bin/"
    DESTINATION "${CMAKE_INSTALL_BINDIR}"
    FILES_MATCHING
    PATTERN "tesseract_*.dll"
    PATTERN "boost_plugin_loader*.dll")
endif()

# ─── Umbrella package config ──────────────────────────────────────────────────
# Generates lib/cmake/CellForge/CellForge-config.cmake so consumers can: find_package(CellForge REQUIRED COMPONENTS
# viewer) target_link_libraries(myapp PRIVATE CellForge::viewer)
#
# configure_package() (from RICB) generates an umbrella config that includes
# ${CMAKE_CURRENT_LIST_DIR}/${component}-config.cmake for each component. Each component installs its own config to
# lib/cmake/CellForge/ so the relative include resolves correctly.
configure_package(COMPONENT CellForge SUPPORTED_COMPONENTS ${SUPPORTED_COMPONENTS})

# ─── Version file ─────────────────────────────────────────────────────────────
# RICB's configure_package() writes ExactVersion; override with SameMajorVersion so consumers can do
# find_package(CellForge 0.1) and get any 0.x release.
include(CMakePackageConfigHelpers)
write_basic_package_version_file("${CMAKE_BINARY_DIR}/CellForge-config-version.cmake" VERSION "${PROJECT_VERSION}"
                                 COMPATIBILITY SameMajorVersion)

# ─── CPack / MSI installer ────────────────────────────────────────────────────
set(CPACK_PACKAGE_NAME "CellForge")
set(CPACK_PACKAGE_VENDOR "Melina Aero Teknoloji Gelistirme ve Dizayn Burosu A.S.")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "CellForge SDK — Qt6/OCCT visualisation and Tesseract motion planning")
set(CPACK_PACKAGE_VERSION "${PROJECT_VERSION}")
set(CPACK_PACKAGE_VERSION_MAJOR "${PROJECT_VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${PROJECT_VERSION_MINOR}")
set(CPACK_PACKAGE_VERSION_PATCH "${PROJECT_VERSION_PATCH}")
set(CPACK_PACKAGE_INSTALL_DIRECTORY "CellForge")

if(WIN32)
  # WiX's license control only accepts .txt or .rtf files; LICENSE has no extension.
  configure_file("${CMAKE_SOURCE_DIR}/LICENSE" "${CMAKE_BINARY_DIR}/LICENSE.txt" COPYONLY)
  set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_BINARY_DIR}/LICENSE.txt")
else()
  set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_SOURCE_DIR}/LICENSE")
endif()

if(WIN32)
  set(CPACK_GENERATOR "WIX")
  # Stable product GUID — must NOT change between releases so WiX can match an existing installation and perform an
  # in-place upgrade instead of a parallel install.  Regenerate only when intentionally breaking upgrades.
  set(CPACK_WIX_UPGRADE_GUID "6E5D4C3B-2A19-4870-B6F5-E4D3C2B1A091")
else()
  set(CPACK_GENERATOR "TGZ")
endif()

include(CPack)

# ─── Integration tests (ExternalProject — tests against installed tesseract) ──
include(cmake/IntegrationTests.cmake)
