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
    CXX_STANDARD          17
    CXX_STANDARD_REQUIRED ON
    OUTPUT_NAME           CellForgeViewerExample
)
