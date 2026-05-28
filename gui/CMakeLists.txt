cmake_minimum_required(VERSION 3.18)

find_package(ros_industrial_cmake_boilerplate REQUIRED)
extract_package_metadata(pkg)
project(${pkg_extracted_name} VERSION ${pkg_extracted_version} LANGUAGES CXX)

if(WIN32)
  set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
endif()

# CellForge-wide compiler settings (CXX version, warning flags, definitions)
include(${CMAKE_CURRENT_SOURCE_DIR}/../cmake/cellforge_macros.cmake)
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../cmake/")
cellforge_variables()

# ─── Options ─────────────────────────────────────────────────────────────────
option(CELLFORGE_VIEWER_BUILD_EXAMPLES "Build viewer example application" OFF)
option(CELLFORGE_VIEWER_BUILD_TESTS    "Build viewer unit tests"          OFF)

# ─── Dependencies ─────────────────────────────────────────────────────────────
find_package(Qt6         REQUIRED COMPONENTS Widgets)
find_package(OpenCASCADE REQUIRED)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

# ─── Library ─────────────────────────────────────────────────────────────────
add_library(viewer SHARED
    src/Viewer.cpp
    src/ViewerInteractor.cpp
    src/ViewerWidget.cpp
    # List the Q_OBJECT header explicitly so AUTOMOC always finds and MOC-processes it,
    # even though it lives in include/CellForge/ rather than next to the .cpp files.
    include/CellForge/ViewerWidget.h
)
add_library(CellForge::viewer ALIAS viewer)
set_target_properties(viewer PROPERTIES OUTPUT_NAME cellforge_viewer)

target_cxx_version(viewer PUBLIC VERSION ${CELLFORGE_CXX_VERSION})
target_compile_options(viewer PRIVATE ${CELLFORGE_COMPILE_OPTIONS_PRIVATE})
target_compile_options(viewer PUBLIC  ${CELLFORGE_COMPILE_OPTIONS_PUBLIC})
target_compile_definitions(viewer PUBLIC ${CELLFORGE_COMPILE_DEFINITIONS})

target_include_directories(viewer
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
        # OCCT headers are part of the public API (Viewer.h includes them),
        # so consumers must be able to find them too.
        $<BUILD_INTERFACE:${OpenCASCADE_INCLUDE_DIR}>
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/include/CellForge
)

target_link_libraries(viewer
    PUBLIC
        CellForge::core
        Qt6::Widgets
    PRIVATE
        TKernel     # Standard_Transient, Handle
        TKMath      # gp_*, TColgp_*
        TKService   # Aspect_*, Graphic3d_*, WNT_Window (Windows)
        TKV3d       # V3d_Viewer, V3d_View, AIS_InteractiveContext, AIS_ViewController
        TKBRep      # TopoDS_Shape
        TKOpenGl    # OpenGL rendering driver
)

# ─── Install & CMake package export ───────────────────────────────────────────
# All CellForge component configs install to lib/cmake/CellForge/ so the root
# umbrella configure_package() can include them via a single relative path.
include(CMakePackageConfigHelpers)
include(GNUInstallDirs)

install(TARGETS viewer
    EXPORT   CellForge-viewer-targets
    RUNTIME  DESTINATION "${CMAKE_INSTALL_BINDIR}"
    LIBRARY  DESTINATION "${CMAKE_INSTALL_LIBDIR}"
    ARCHIVE  DESTINATION "${CMAKE_INSTALL_LIBDIR}"
)
install(EXPORT CellForge-viewer-targets
    NAMESPACE   CellForge::
    DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/CellForge"
    FILE        viewer-targets.cmake
)

# Generate viewer-config.cmake that finds dependencies and imports targets
file(WRITE "${CMAKE_CURRENT_BINARY_DIR}/viewer-config.cmake.in"
"@PACKAGE_INIT@
include(CMakeFindDependencyMacro)
find_dependency(Qt6 REQUIRED COMPONENTS Widgets)
find_dependency(OpenCASCADE REQUIRED)
if(NOT TARGET CellForge::viewer)
  include(\"\${CMAKE_CURRENT_LIST_DIR}/viewer-targets.cmake\")
endif()
")
configure_package_config_file(
    "${CMAKE_CURRENT_BINARY_DIR}/viewer-config.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/viewer-config.cmake"
    INSTALL_DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/CellForge"
    NO_CHECK_REQUIRED_COMPONENTS_MACRO
)
install(FILES "${CMAKE_CURRENT_BINARY_DIR}/viewer-config.cmake"
    DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/CellForge"
)

install(
    DIRECTORY   include/CellForge
    DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}"
    FILES_MATCHING PATTERN "*.h"
)

# Register with the root umbrella (propagates to root's SUPPORTED_COMPONENTS)
set(SUPPORTED_COMPONENTS ${SUPPORTED_COMPONENTS} "viewer" PARENT_SCOPE)

# ─── Sub-directories ─────────────────────────────────────────────────────────
if(CELLFORGE_VIEWER_BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()

if(CELLFORGE_VIEWER_BUILD_TESTS)
    add_subdirectory(test)
endif()
