# cmake/pinocchio_toolchain.cmake
#
# Minimal toolchain used exclusively by the vendor_pinocchio ExternalProject.
#
# WHY this file exists:
#   vcpkg's buildsystems/vcpkg.cmake overrides three cmake commands:
#     find_package  (macro, line ~788)
#     add_executable (function, line ~610)
#     add_library    (function, line ~651)
#   pinocchio 4.x uses jrl-cmakemodules which also redefines those same
#   commands.  The second redefinition makes cmake alias the first definition
#   as _<cmd>, so vcpkg's function now calls _add_executable which IS itself.
#   Result: 1000-level infinite recursion.
#
#   The fix: do not include vcpkg.cmake at all for pinocchio.
#   Instead, add the vcpkg install tree to CMAKE_PREFIX_PATH so that
#   config-mode find_package calls still find Eigen3, Boost, urdfdom, etc.
#   All of those packages ship proper *Config.cmake files in the vcpkg store,
#   so no vcpkg hook is needed.

# --- Locate the vcpkg install tree ------------------------------------
if(NOT DEFINED _Z_PINOCCHIO_VCPKG_INSTALLED_DIR)
  if(DEFINED VCPKG_INSTALLED_DIR AND EXISTS "${VCPKG_INSTALLED_DIR}")
    set(_Z_PINOCCHIO_VCPKG_INSTALLED_DIR "${VCPKG_INSTALLED_DIR}")
  elseif(DEFINED ENV{VCPKG_ROOT} AND EXISTS "$ENV{VCPKG_ROOT}/installed")
    set(_Z_PINOCCHIO_VCPKG_INSTALLED_DIR "$ENV{VCPKG_ROOT}/installed")
  endif()
endif()

if(NOT DEFINED VCPKG_TARGET_TRIPLET)
  set(VCPKG_TARGET_TRIPLET "x64-windows-release")
endif()

if(DEFINED _Z_PINOCCHIO_VCPKG_INSTALLED_DIR
   AND EXISTS "${_Z_PINOCCHIO_VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}")

  set(_Z_PINOCCHIO_PREFIX "${_Z_PINOCCHIO_VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}")

  # Prepend so our packages take priority over any system ones.
  list(PREPEND CMAKE_PREFIX_PATH "${_Z_PINOCCHIO_PREFIX}")
  list(PREPEND CMAKE_LIBRARY_PATH "${_Z_PINOCCHIO_PREFIX}/lib")
  list(PREPEND CMAKE_INCLUDE_PATH "${_Z_PINOCCHIO_PREFIX}/include")
  if(WIN32)
    list(PREPEND CMAKE_PROGRAM_PATH "${_Z_PINOCCHIO_PREFIX}/bin")
    list(PREPEND CMAKE_PROGRAM_PATH "${_Z_PINOCCHIO_PREFIX}/tools")
  endif()

  # Let cmake find *.cmake config files in the share/* subdirectories.
  set(CMAKE_FIND_ROOT_PATH "${_Z_PINOCCHIO_PREFIX}" CACHE PATH "vcpkg root path for pinocchio sub-build" FORCE)
  set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY BOTH CACHE STRING "" FORCE)
  set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE BOTH CACHE STRING "" FORCE)
  set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE BOTH CACHE STRING "" FORCE)

  # Mark as a vcpkg-aware configure so downstream checks pass.
  set(VCPKG_TOOLCHAIN ON)
endif()
