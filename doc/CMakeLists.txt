# ------------------------------------------------------------------------------------
# Project: CellForge
# Copyright (C) 2026, Melina Aero Teknoloji Gelistirme ve Dizayn Burosu A.S., Istanbul
# Author: Onur Tuncer, PhD
#
# SPDX-License-Identifier: LGPL-2.1-only
# License-Filename: LICENSE
# ------------------------------------------------------------------------------------

cmake_minimum_required(VERSION 3.21)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../cmake")

find_package(Doxygen REQUIRED)
find_package(Sphinx REQUIRED)

set(PROJECT_DOC_NAME ${CMAKE_PROJECT_NAME}_Docs)

# ─── Doxygen (API reference, consumed by Sphinx via breathe) ──────────────────
set(DOXYGEN_OUTPUT_DIR "${CMAKE_CURRENT_BINARY_DIR}/doxygen")
set(DOXYGEN_INDEX_FILE "${DOXYGEN_OUTPUT_DIR}/xml/index.xml")
set(DOXYFILE_IN "${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in")
set(DOXYFILE_OUT "${CMAKE_CURRENT_BINARY_DIR}/Doxyfile")

configure_file("${DOXYFILE_IN}" "${DOXYFILE_OUT}" @ONLY)
file(MAKE_DIRECTORY "${DOXYGEN_OUTPUT_DIR}")

# Only regenerate Doxygen when the Doxyfile or public headers change.
file(
  GLOB_RECURSE PROJECT_PUBLIC_HEADERS
  "${CMAKE_SOURCE_DIR}/core/include/*.h"
  "${CMAKE_SOURCE_DIR}/workcell/include/*.h"
  "${CMAKE_SOURCE_DIR}/viewer/include/*.h"
  "${CMAKE_SOURCE_DIR}/platform/include/*.h"
  "${CMAKE_SOURCE_DIR}/persistence/include/*.h"
  "${CMAKE_SOURCE_DIR}/project/include/*.h")

add_custom_command(
  OUTPUT "${DOXYGEN_INDEX_FILE}"
  COMMAND "${DOXYGEN_EXECUTABLE}" "${DOXYFILE_OUT}"
  MAIN_DEPENDENCY "${DOXYFILE_OUT}"
  DEPENDS ${PROJECT_PUBLIC_HEADERS} "${DOXYFILE_IN}"
  WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}"
  COMMENT "Generating ${PROJECT_DOC_NAME} Doxygen XML"
  VERBATIM)

add_custom_target(${PROJECT_DOC_NAME}_Doxygen ALL DEPENDS "${DOXYGEN_INDEX_FILE}")

# ─── Sphinx (HTML site, embeds the Doxygen API reference via breathe) ─────────
set(SPHINX_SOURCE "${CMAKE_CURRENT_SOURCE_DIR}")
set(SPHINX_BUILD "${CMAKE_CURRENT_BINARY_DIR}/sphinx")
set(SPHINX_INDEX_FILE "${SPHINX_BUILD}/index.html")

# Only regenerate Sphinx when: Doxygen has rerun, our doc sources changed, or conf.py changed.
add_custom_command(
  OUTPUT "${SPHINX_INDEX_FILE}"
  COMMAND "${SPHINX_EXECUTABLE}" -b html "-Dbreathe_projects.${CMAKE_PROJECT_NAME}=${DOXYGEN_OUTPUT_DIR}/xml"
          "${SPHINX_SOURCE}" "${SPHINX_BUILD}"
  WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}"
  DEPENDS "${SPHINX_SOURCE}/index.rst" "${SPHINX_SOURCE}/intro.rst" "${DOXYGEN_INDEX_FILE}"
  MAIN_DEPENDENCY "${SPHINX_SOURCE}/conf.py"
  COMMENT "Generating ${PROJECT_DOC_NAME} HTML documentation with Sphinx"
  VERBATIM)

add_custom_target(${PROJECT_DOC_NAME}_Sphinx ALL DEPENDS "${SPHINX_INDEX_FILE}")

# ─── Install (bundled into CellForge installers, e.g. the WiX MSI) ────────────
include(GNUInstallDirs)
install(DIRECTORY "${SPHINX_BUILD}/" DESTINATION "${CMAKE_INSTALL_DOCDIR}")
