cmake_minimum_required(VERSION 3.18)
project(cellforge_platform_mfc_examples LANGUAGES CXX)

add_executable(example_mfc_event_log
    main.cpp
)

target_compile_features(example_mfc_event_log PRIVATE cxx_std_20)

target_compile_definitions(example_mfc_event_log PRIVATE
    CF_PLATFORM_WINDOWS
    _AFXDLL
    UNICODE
    _UNICODE
    WIN32_LEAN_AND_MEAN
)

# VS project must be set to "Use MFC in a Shared DLL" or the linker won't find
# the MFC entry-point symbol that replaces the standard CRT one.
set_property(TARGET example_mfc_event_log PROPERTY VS_GLOBAL_UseOfMfc "2")

target_link_libraries(example_mfc_event_log
    PRIVATE
        CellForge::platform_mfc
        CellForge::core
)

# ─── Runtime deployment (Windows) ─────────────────────────────────────────────
add_custom_command(TARGET example_mfc_event_log POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "$<TARGET_FILE:cellforge_core>"
        "$<TARGET_FILE:cellforge_platform_mfc>"
        "$<TARGET_FILE_DIR:example_mfc_event_log>"
    COMMENT "Deploying CellForge DLLs alongside MFC example"
)
