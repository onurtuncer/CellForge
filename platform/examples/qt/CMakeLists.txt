# ------------------------------------------------------------------------------------
# Project: CellForge
# Copyright (C) 2026, Melina Aero Teknoloji Gelistirme ve Dizayn Burosu A.S., Istanbul
# Author: Onur Tuncer, PhD
#
# SPDX-License-Identifier: LGPL-2.1-only
# License-Filename: LICENSE
# ------------------------------------------------------------------------------------

cmake_minimum_required(VERSION 3.18)
project(cellforge_platform_qt_examples LANGUAGES CXX)

find_package(Qt6 REQUIRED COMPONENTS Widgets)

set(CMAKE_AUTOMOC ON)

add_executable(example_event_log
    main.cpp
)

target_compile_features(example_event_log PRIVATE cxx_std_20)

if(WIN32)
    target_compile_definitions(example_event_log PRIVATE CF_PLATFORM_WINDOWS)
elseif(UNIX)
    target_compile_definitions(example_event_log PRIVATE CF_PLATFORM_LINUX)
endif()

target_link_libraries(example_event_log
    PRIVATE
        CellForge::platform_qt
        CellForge::core
        Qt6::Widgets
)

# ─── Runtime deployment (Windows) ─────────────────────────────────────────────
if(WIN32)
    # Derive Qt bin dir from the imported Qt6::Core DLL location.
    get_target_property(_qt6_dll Qt6::Core IMPORTED_LOCATION_RELEASE)
    get_filename_component(_qt6_bin  "${_qt6_dll}" DIRECTORY)   # .../bin
    get_filename_component(_qt6_root "${_qt6_bin}"  DIRECTORY)  # .../<prefix>

    # qt.conf tells Qt where to find its platform plugin (qwindows.dll) without
    # requiring QT_PLUGIN_PATH to be set in the environment.
    file(GENERATE
        OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/qt.conf"
        CONTENT "[Paths]\nPlugins = ${_qt6_root}/Qt6/plugins\n"
    )
    add_custom_command(TARGET example_event_log POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${CMAKE_CURRENT_BINARY_DIR}/qt.conf"
            "$<TARGET_FILE_DIR:example_event_log>/qt.conf"
        COMMENT "Deploying qt.conf"
    )

    # CellForge DLLs — use generator expressions so they track the build config.
    add_custom_command(TARGET example_event_log POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "$<TARGET_FILE:cellforge_core>"
            "$<TARGET_FILE:cellforge_platform_qt>"
            "$<TARGET_FILE_DIR:example_event_log>"
        COMMENT "Deploying CellForge DLLs"
    )

    # Qt DLLs and their dependencies (ICU, pcre2, freetype, etc.) live in the
    # same bin directory as Qt6Core.dll — copy the whole set.
    file(GLOB _qt_bin_dlls "${_qt6_bin}/*.dll")
    add_custom_command(TARGET example_event_log POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            ${_qt_bin_dlls}
            "$<TARGET_FILE_DIR:example_event_log>"
        COMMENT "Deploying Qt6 DLLs from ${_qt6_bin}"
    )
endif()
