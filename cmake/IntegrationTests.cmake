option(CELLFORGE_BUILD_TESTS "Build CellForge integration tests" ON)

if(NOT CELLFORGE_BUILD_TESTS)
  return()
endif()

# Test project builds as a separate ExternalProject so it can consume the
# installed tesseract packages from VENDOR_INSTALL_PREFIX via find_package().
# It deliberately skips the install step — test binaries stay in the build tree.
#
# _VENDOR_ARGS and VENDOR_INSTALL_PREFIX are defined in VendorDependencies.cmake,
# which is included before this file in the top-level CMakeLists.txt.

set(_TESTS_BUILD_DIR "${CMAKE_BINARY_DIR}/tests_build")

ExternalProject_Add(
  cellforge_integration_tests
  SOURCE_DIR "${CMAKE_SOURCE_DIR}/tests"
  BINARY_DIR "${_TESTS_BUILD_DIR}"
  CMAKE_ARGS "-DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}"
             "-DCMAKE_TOOLCHAIN_FILE=${CMAKE_TOOLCHAIN_FILE}"
             "-DCMAKE_PREFIX_PATH=${VENDOR_INSTALL_PREFIX}"
             "-DVCPKG_TARGET_TRIPLET=${VCPKG_TARGET_TRIPLET}"
             "-DVCPKG_OVERLAY_TRIPLETS=${VCPKG_OVERLAY_TRIPLETS}"
             "-DCMAKE_CXX_STANDARD=17"
             "-DCMAKE_WINDOWS_EXPORT_ALL_SYMBOLS=ON"
             "-DCMAKE_EXPORT_COMPILE_COMMANDS=ON"
  INSTALL_COMMAND ""
  BUILD_ALWAYS OFF
  DEPENDS vendor_tesseract)

# Register a top-level CTest entry that delegates to the test sub-build. Run `ctest` (or `cmake --build . --target
# RUN_TESTS`) after a full build.
add_test(NAME CellForge.Integration COMMAND ${CMAKE_CTEST_COMMAND} --output-on-failure -C $<CONFIG> --test-dir
                                            "${_TESTS_BUILD_DIR}")
