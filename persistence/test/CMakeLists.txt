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

add_executable(cellforge_persistence_tests
    test_document.cpp
    test_nodes.cpp
    test_attributes.cpp
    test_undo_redo.cpp
)

target_link_libraries(cellforge_persistence_tests
    PRIVATE
        CellForge::persistence
        Catch2::Catch2WithMain
)

target_compile_features(cellforge_persistence_tests PRIVATE cxx_std_20)
target_compile_options(cellforge_persistence_tests PRIVATE ${CELLFORGE_COMPILE_OPTIONS_PRIVATE})

if(WIN32)
    # cellforge_persistence.dll must live next to the test executable at runtime.
    add_custom_command(TARGET cellforge_persistence_tests POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            $<TARGET_FILE:persistence>
            $<TARGET_FILE_DIR:cellforge_persistence_tests>
        COMMENT "Copying cellforge_persistence.dll alongside tests"
    )
endif()

catch_discover_tests(cellforge_persistence_tests)
