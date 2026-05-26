cmake_minimum_required(VERSION 3.18)

find_package(ros_industrial_cmake_boilerplate REQUIRED)
project(cellforge_platform_qt VERSION 0.1.0 LANGUAGES CXX)

if(WIN32)
  set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
endif()

include(${CMAKE_CURRENT_SOURCE_DIR}/../cmake/cellforge_macros.cmake)
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../cmake/")
cellforge_variables()

# ─── Dependencies ─────────────────────────────────────────────────────────────
find_package(Qt6 REQUIRED COMPONENTS Widgets)

set(CMAKE_AUTOMOC ON)

# ─── Library ─────────────────────────────────────────────────────────────────
add_library(cellforge_platform_qt SHARED
    src/ViewPortWidget.cpp
    # Listed explicitly so AUTOMOC processes Q_OBJECT in the include tree.
    include/CellForge/qt/widgets/ViewPortWidget.h
)
add_library(CellForge::platform_qt ALIAS cellforge_platform_qt)
set_target_properties(cellforge_platform_qt PROPERTIES OUTPUT_NAME cellforge_platform_qt)

target_cxx_version(cellforge_platform_qt PUBLIC VERSION ${CELLFORGE_CXX_VERSION})
target_compile_options(cellforge_platform_qt PRIVATE ${CELLFORGE_COMPILE_OPTIONS_PRIVATE})
target_compile_options(cellforge_platform_qt PUBLIC  ${CELLFORGE_COMPILE_OPTIONS_PUBLIC})
target_compile_definitions(cellforge_platform_qt PUBLIC ${CELLFORGE_COMPILE_DEFINITIONS})

if(WIN32)
    target_compile_definitions(cellforge_platform_qt PUBLIC CF_PLATFORM_WINDOWS)
elseif(UNIX)
    target_compile_definitions(cellforge_platform_qt PUBLIC CF_PLATFORM_LINUX)
endif()

target_include_directories(cellforge_platform_qt
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/include/CellForge
)

target_link_libraries(cellforge_platform_qt
    PUBLIC
        CellForge::core
        Qt6::Widgets
)

# ─── Install & CMake package export ───────────────────────────────────────────
include(CMakePackageConfigHelpers)
include(GNUInstallDirs)

install(TARGETS cellforge_platform_qt
    EXPORT   CellForge-platform_qt-targets
    RUNTIME  DESTINATION "${CMAKE_INSTALL_BINDIR}"
    LIBRARY  DESTINATION "${CMAKE_INSTALL_LIBDIR}"
    ARCHIVE  DESTINATION "${CMAKE_INSTALL_LIBDIR}"
)
install(EXPORT CellForge-platform_qt-targets
    NAMESPACE   CellForge::
    DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/CellForge"
    FILE        platform_qt-targets.cmake
)

file(WRITE "${CMAKE_CURRENT_BINARY_DIR}/platform_qt-config.cmake.in"
"@PACKAGE_INIT@
include(CMakeFindDependencyMacro)
find_dependency(Qt6 REQUIRED COMPONENTS Widgets)
find_dependency(CellForge REQUIRED COMPONENTS core)
if(NOT TARGET CellForge::platform_qt)
  include(\"\${CMAKE_CURRENT_LIST_DIR}/platform_qt-targets.cmake\")
endif()
")
configure_package_config_file(
    "${CMAKE_CURRENT_BINARY_DIR}/platform_qt-config.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/platform_qt-config.cmake"
    INSTALL_DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/CellForge"
    NO_CHECK_REQUIRED_COMPONENTS_MACRO
)
install(FILES "${CMAKE_CURRENT_BINARY_DIR}/platform_qt-config.cmake"
    DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/CellForge"
)

install(
    DIRECTORY   include/CellForge
    DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}"
    FILES_MATCHING PATTERN "*.h"
)

set(SUPPORTED_COMPONENTS ${SUPPORTED_COMPONENTS} "platform_qt" PARENT_SCOPE)

# ─── Sub-directories ─────────────────────────────────────────────────────────
option(CELLFORGE_PLATFORM_QT_BUILD_EXAMPLES "Build platform_qt example application" OFF)
if(CELLFORGE_PLATFORM_QT_BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()
