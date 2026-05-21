# CellForge first-party DLL registry.
#
# Add a new component by:
#   1. Creating <name>/CMakeLists.txt + <name>/package.xml.
#   2. Adding an option + add_subdirectory block below.
#
# Each component must end with:
#   set(SUPPORTED_COMPONENTS ${SUPPORTED_COMPONENTS} "<name>" PARENT_SCOPE)
# so the root configure_package() picks it up.
#
# Components that need a vendor package (e.g. tesseract) must add the
# corresponding vendor ExternalProject target to their own build step via a
# CMake dependency (add_dependencies) or by checking at configure time that
# the vendor headers exist.  For now, viewer only needs vcpkg packages
# (Qt6, OpenCASCADE) which are available at configure time.

option(CELLFORGE_BUILD_VIEWER          "Build Qt6/OCCT viewer library" ON)
option(CELLFORGE_BUILD_VIEWER_EXAMPLES "Build viewer example app"      OFF)

if(CELLFORGE_BUILD_VIEWER)
  add_subdirectory(qt-viewer)
endif()
