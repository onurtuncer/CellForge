cmake_minimum_required(VERSION 3.18)

# ros_industrial_cmake_boilerplate must be installed before this package.
# extract_package_metadata() reads package.xml to derive the project name/version.
find_package(ros_industrial_cmake_boilerplate REQUIRED)
extract_package_metadata(pkg)
project(${pkg_extracted_name} VERSION ${pkg_extracted_version} LANGUAGES CXX)

if(WIN32)
  set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
endif()

# ─── Options ─────────────────────────────────────────────────────────────────
option(CELLFORGE_VIEWER_BUILD_EXAMPLES "Build viewer example application" OFF)
option(CELLFORGE_VIEWER_BUILD_TESTS    "Build viewer unit tests"          OFF)

# ─── Dependencies ─────────────────────────────────────────────────────────────
find_package(Qt6         REQUIRED COMPONENTS Widgets)
find_package(OpenCASCADE REQUIRED)

# CMake drives MOC/RCC automatically for all targets in this directory
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

# ─── Library ─────────────────────────────────────────────────────────────────
add_library(viewer SHARED
    src/Viewer.cpp
    src/ViewerInteractor.cpp
    src/ViewerWidget.cpp
)
add_library(CellForge::viewer ALIAS viewer)
set_target_properties(viewer PROPERTIES OUTPUT_NAME cellforge_viewer)

target_compile_features(viewer PUBLIC cxx_std_17)

target_include_directories(viewer
    PUBLIC
        # Installed consumers: #include <CellForge/Viewer.h>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
    PRIVATE
        # Internal .cpp files may use flat includes: #include "Viewer.h"
        ${CMAKE_CURRENT_SOURCE_DIR}/include/CellForge
        ${OpenCASCADE_INCLUDE_DIR}
)

target_link_libraries(viewer
    PUBLIC
        Qt6::Widgets
    PRIVATE
        TKernel         # Standard_Transient, Handle, TCollection_AsciiString
        TKMath          # gp_*, TColgp_*
        TKService       # Aspect_*, Graphic3d_*
        TKV3d           # V3d_Viewer, V3d_View
        TKAIS           # AIS_InteractiveContext, AIS_ViewController
        TKBRep          # TopoDS_Shape
        TKOpenGl        # OpenGL rendering driver
        $<$<PLATFORM_ID:Windows>:TKWNT>   # WNT_Window — Windows only
)

# ─── Package export (tesseract / RICB scheme) ─────────────────────────────────
# Installs:
#   lib/cmake/cellforge_viewer/viewer-config.cmake
#   lib/cmake/cellforge_viewer/viewer-targets.cmake
# Consumers: find_package(cellforge_viewer REQUIRED COMPONENTS viewer)
#            target_link_libraries(... CellForge::viewer)
configure_component(
    COMPONENT viewer
    NAMESPACE CellForge
    TARGETS   viewer
    DEPENDENCIES
        "Qt6 COMPONENTS Widgets"
        OpenCASCADE
)

install(
    DIRECTORY   include/CellForge
    DESTINATION include
    COMPONENT   viewer
    FILES_MATCHING PATTERN "*.h"
)

# ─── Sub-directories ─────────────────────────────────────────────────────────
if(CELLFORGE_VIEWER_BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()

if(CELLFORGE_VIEWER_BUILD_TESTS)
    add_subdirectory(test)
endif()
