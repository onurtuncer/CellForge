# ------------------------------------------------------------------------------------
# Project: CellForge
# Copyright (C) 2026, Melina Aero Teknoloji Gelistirme ve Dizayn Burosu A.S., Istanbul
# Author: Onur Tuncer, PhD
#
# SPDX-License-Identifier: LGPL-2.1-only
# License-Filename: LICENSE
# ------------------------------------------------------------------------------------

# CMAKE_MFC_FLAG must be set in its own subdirectory so the VS generator applies
# UseOfMfc=Dynamic only to the targets defined here.  Setting it in the parent
# directory would propagate it to the Qt library, which doesn't need MFC headers.
set(CMAKE_MFC_FLAG 2)   # 2 = Use MFC in a Shared DLL

add_library(cellforge_platform_mfc SHARED
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/mfc/MfcApplicationPlatform.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/mfc/MfcViewportWnd.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/mfc/MfcViewerWnd.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/mfc/MfcWindow.cpp
)
add_library(CellForge::platform_mfc ALIAS cellforge_platform_mfc)
set_target_properties(cellforge_platform_mfc PROPERTIES OUTPUT_NAME cellforge_platform_mfc)

target_cxx_version(cellforge_platform_mfc PUBLIC VERSION ${CELLFORGE_CXX_VERSION})
target_compile_options(cellforge_platform_mfc PRIVATE ${CELLFORGE_COMPILE_OPTIONS_PRIVATE})
target_compile_options(cellforge_platform_mfc PUBLIC  ${CELLFORGE_COMPILE_OPTIONS_PUBLIC})
target_compile_definitions(cellforge_platform_mfc PUBLIC ${CELLFORGE_COMPILE_DEFINITIONS})

target_compile_definitions(cellforge_platform_mfc PUBLIC
    CF_PLATFORM_WINDOWS
    _AFXDLL     # shared MFC DLL
    UNICODE
    _UNICODE
)

target_compile_definitions(cellforge_platform_mfc PUBLIC
    $<$<CONFIG:Debug>:CF_DEBUG>
    $<$<CONFIG:Release>:CF_RELEASE>
    $<$<CONFIG:RelWithDebInfo>:CF_RELEASE>
    $<$<CONFIG:MinSizeRel>:CF_DIST>
)

target_include_directories(cellforge_platform_mfc
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../include>
        $<INSTALL_INTERFACE:include>
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/../include/CellForge
)

target_link_libraries(cellforge_platform_mfc
    PUBLIC
        CellForge::core
        CellForge::viewer   # IViewerWidget.h is a public header dep
    PRIVATE
        TKernel     # Standard_*, OSD_*
        TKV3d       # AIS_*, PrsMgr_*, SelectMgr_*, V3d_*
        TKService   # Aspect_*
        TKBRep      # TopoDS_*
)

# ─── Install & CMake package export ───────────────────────────────────────────
include(CMakePackageConfigHelpers)
include(GNUInstallDirs)

install(TARGETS cellforge_platform_mfc
    EXPORT                  CellForge-platform_mfc-targets
    RUNTIME_DEPENDENCY_SET  CF_runtime_deps
    RUNTIME  DESTINATION "${CMAKE_INSTALL_BINDIR}"
    LIBRARY  DESTINATION "${CMAKE_INSTALL_LIBDIR}"
    ARCHIVE  DESTINATION "${CMAKE_INSTALL_LIBDIR}"
)
install(EXPORT CellForge-platform_mfc-targets
    NAMESPACE   CellForge::
    DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/CellForge"
    FILE        platform_mfc-targets.cmake
)

file(WRITE "${CMAKE_CURRENT_BINARY_DIR}/platform_mfc-config.cmake.in"
"@PACKAGE_INIT@
include(CMakeFindDependencyMacro)
find_dependency(CellForge REQUIRED COMPONENTS core viewer)
if(NOT TARGET CellForge::platform_mfc)
  include(\"\${CMAKE_CURRENT_LIST_DIR}/platform_mfc-targets.cmake\")
endif()
")
configure_package_config_file(
    "${CMAKE_CURRENT_BINARY_DIR}/platform_mfc-config.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/platform_mfc-config.cmake"
    INSTALL_DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/CellForge"
    NO_CHECK_REQUIRED_COMPONENTS_MACRO
)
install(FILES "${CMAKE_CURRENT_BINARY_DIR}/platform_mfc-config.cmake"
    DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/CellForge"
)

set(SUPPORTED_COMPONENTS ${SUPPORTED_COMPONENTS} "platform_mfc" PARENT_SCOPE)
