cmake_minimum_required(VERSION 3.18)

include(cmake/Vcpkg.cmake)

project(CellForge VERSION 0.1.0 LANGUAGES C CXX)

include(cmake/VendorDependencies.cmake)
include(cmake/IntegrationTests.cmake)

# ─── CellForge application code ──────────────────────────────────────────────
# Vendor packages are staged in VENDOR_INSTALL_PREFIX after the superbuild.
# Consume them with find_package() once you have source targets to build:
#
#   list(APPEND CMAKE_PREFIX_PATH "${VENDOR_INSTALL_PREFIX}")
#   find_package(tesseract_environment REQUIRED)
#   add_subdirectory(src)
