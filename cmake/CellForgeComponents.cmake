# CellForge first-party DLLs built as ExternalProjects.
#
# Each component follows the same scheme as the tesseract vendor packages:
#   - package.xml  → name / version metadata (read by extract_package_metadata)
#   - CMakeLists.txt uses configure_component() from ros_industrial_cmake_boilerplate
#   - Installed to VENDOR_INSTALL_PREFIX so subsequent components can find each other
#     via find_package() using CMAKE_PREFIX_PATH=${VENDOR_INSTALL_PREFIX}
#
# _VENDOR_ARGS and VENDOR_INSTALL_PREFIX are defined in VendorDependencies.cmake,
# which is included before this file in the top-level CMakeLists.txt.
#
# To add a new DLL:
#   1. Create <name>/CMakeLists.txt + <name>/package.xml following the pattern below.
#   2. Add an ExternalProject_Add block here with DEPENDS on any sibling components it needs.

# ─── cellforge_viewer ─────────────────────────────────────────────────────────
option(CELLFORGE_BUILD_VIEWER          "Build the Qt6/OCCT viewer library"   ON)
option(CELLFORGE_BUILD_VIEWER_EXAMPLES "Build viewer example application"    OFF)

if(CELLFORGE_BUILD_VIEWER)
    ExternalProject_Add(cellforge_viewer
        SOURCE_DIR  "${CMAKE_SOURCE_DIR}/qt-viewer"
        INSTALL_DIR "${VENDOR_INSTALL_PREFIX}"
        CMAKE_ARGS
            ${_VENDOR_ARGS}
            "-DCELLFORGE_VIEWER_BUILD_EXAMPLES=${CELLFORGE_BUILD_VIEWER_EXAMPLES}"
            "-DCELLFORGE_VIEWER_BUILD_TESTS=OFF"
        BUILD_ALWAYS OFF
        # Needs ros_industrial_cmake_boilerplate macros; add vendor_tesseract here
        # when the viewer grows a dependency on tesseract.
        DEPENDS vendor_ricb
    )
endif()
