add_executable(cellforge_viewer_example main.cpp)

target_link_libraries(cellforge_viewer_example
    PRIVATE
        CellForge::viewer
        Qt6::Widgets
)

# main.cpp uses flat #include "ViewerWidget.h"; expose the CellForge header
# subdirectory directly so the directive resolves without editing the file.
target_include_directories(cellforge_viewer_example PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/../include/CellForge
)

set_target_properties(cellforge_viewer_example PROPERTIES
    CXX_STANDARD          20
    CXX_STANDARD_REQUIRED ON
    OUTPUT_NAME           CellForgeViewerExample
)

# ─── Runtime deployment (Windows) ─────────────────────────────────────────────
if(WIN32)
    # Derive vcpkg bin dir from Qt6::Core (Qt, OCCT, spdlog, etc. all live here).
    get_target_property(_qt6_dll Qt6::Core IMPORTED_LOCATION_RELEASE)
    get_filename_component(_qt6_bin  "${_qt6_dll}" DIRECTORY)   # .../bin
    get_filename_component(_qt6_root "${_qt6_bin}"  DIRECTORY)  # .../<prefix>

    # qt.conf tells Qt where to find its platform plugin without QT_PLUGIN_PATH.
    file(GENERATE
        OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/qt.conf"
        CONTENT "[Paths]\nPlugins = ${_qt6_root}/Qt6/plugins\n"
    )
    add_custom_command(TARGET cellforge_viewer_example POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${CMAKE_CURRENT_BINARY_DIR}/qt.conf"
            "$<TARGET_FILE_DIR:cellforge_viewer_example>/qt.conf"
        COMMENT "Deploying qt.conf"
    )

    # CellForge DLLs.
    add_custom_command(TARGET cellforge_viewer_example POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "$<TARGET_FILE:cellforge_core>"
            "$<TARGET_FILE:viewer>"
            "$<TARGET_FILE_DIR:cellforge_viewer_example>"
        COMMENT "Deploying CellForge DLLs"
    )

    # All vcpkg DLLs (Qt6, OCCT, spdlog, yaml-cpp, flecs, TracyClient, …).
    file(GLOB _vcpkg_bin_dlls "${_qt6_bin}/*.dll")
    add_custom_command(TARGET cellforge_viewer_example POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            ${_vcpkg_bin_dlls}
            "$<TARGET_FILE_DIR:cellforge_viewer_example>"
        COMMENT "Deploying vcpkg runtime DLLs from ${_qt6_bin}"
    )
endif()
