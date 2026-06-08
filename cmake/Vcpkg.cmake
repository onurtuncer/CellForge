# Resolves and validates the vcpkg toolchain before project() has been called.
# Sets CMAKE_TOOLCHAIN_FILE, VCPKG_TARGET_TRIPLET, and VCPKG_OVERLAY_TRIPLETS
# as CACHE variables so they propagate into every ExternalProject sub-build.

if(NOT DEFINED CMAKE_TOOLCHAIN_FILE)
  if(DEFINED ENV{VCPKG_ROOT})
    set(CMAKE_TOOLCHAIN_FILE "$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake" CACHE FILEPATH "vcpkg toolchain file")
  else()
    message(
      FATAL_ERROR
        "vcpkg not found.\n" "  Option 1: -DCMAKE_TOOLCHAIN_FILE=<vcpkg_root>/scripts/buildsystems/vcpkg.cmake\n"
        "  Option 2: set the VCPKG_ROOT environment variable")
  endif()
endif()

if(WIN32 AND NOT DEFINED VCPKG_TARGET_TRIPLET)
  set(VCPKG_TARGET_TRIPLET "x64-windows-release" CACHE STRING "vcpkg target triplet")
endif()

set(VCPKG_OVERLAY_TRIPLETS "${CMAKE_SOURCE_DIR}/cmake/triplets" CACHE PATH
                                                                      "Overlay directory for custom vcpkg triplets")
