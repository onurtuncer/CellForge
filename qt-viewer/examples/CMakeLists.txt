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

# ─── Qt plugin deployment ─────────────────────────────────────────────────────
# Place a qt.conf next to the exe so Qt finds its platform plugin (qwindows.dll)
# without QT_PLUGIN_PATH being set in the environment.
get_target_property(_qt6_dll Qt6::Core IMPORTED_LOCATION_RELEASE)
get_filename_component(_qt6_bin "${_qt6_dll}" DIRECTORY)   # .../bin
get_filename_component(_qt6_root "${_qt6_bin}" DIRECTORY)  # .../<prefix>

file(GENERATE
    OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/qt.conf"
    CONTENT "[Paths]\nPlugins = ${_qt6_root}/Qt6/plugins\n"
)

add_custom_command(TARGET cellforge_viewer_example POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy
        "${CMAKE_CURRENT_BINARY_DIR}/qt.conf"
        "$<TARGET_FILE_DIR:cellforge_viewer_example>/qt.conf"
    COMMENT "Deploying qt.conf alongside CellForgeViewerExample"
)
