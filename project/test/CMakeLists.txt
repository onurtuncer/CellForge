# ------------------------------------------------------------------------------------
# Project: CellForge
# Copyright (C) 2026, Melina Aero Teknoloji Gelistirme ve Dizayn Burosu A.S., Istanbul
# Author: Onur Tuncer, PhD
#
# SPDX-License-Identifier: LGPL-2.1-only
# License-Filename: LICENSE
# ------------------------------------------------------------------------------------

find_package(Catch2 3 REQUIRED)
include(Catch)

add_executable(
  cellforge_project_tests
  test_project_lifecycle.cpp
  test_project_items.cpp
  test_project_roundtrip.cpp
  test_project_undo.cpp)

target_link_libraries(
  cellforge_project_tests
  PRIVATE CellForge::project CellForge::persistence # OcafPersistenceBackend — the concrete backend used in tests
          Catch2::Catch2WithMain)

target_compile_features(cellforge_project_tests PRIVATE cxx_std_20)
target_compile_options(cellforge_project_tests PRIVATE ${CELLFORGE_COMPILE_OPTIONS_PRIVATE})

if(WIN32)
  foreach(_dll persistence project)
    add_custom_command(
      TARGET cellforge_project_tests
      POST_BUILD
      COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:${_dll}> $<TARGET_FILE_DIR:cellforge_project_tests>
      COMMENT "Copying cellforge_${_dll}.dll alongside project tests")
  endforeach()
endif()

catch_discover_tests(cellforge_project_tests)
