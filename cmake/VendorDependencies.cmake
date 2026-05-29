include(ExternalProject)

# VENDOR_INSTALL_PREFIX is set by the root CMakeLists.txt before this file
# is included.  It is not re-declared here to keep ownership unambiguous.

# Arguments forwarded to every sub-build.
# ExternalProject_Add inherits the generator/platform/toolset automatically;
# we only need to forward cache variables.
# VCPKG_MANIFEST_MODE=OFF: sub-projects have no vcpkg.json so we explicitly
# disable manifest mode to avoid vcpkg trying (and failing) to run an install.
set(_VENDOR_ARGS
    "-DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}"
    "-DCMAKE_INSTALL_PREFIX=${VENDOR_INSTALL_PREFIX}"
    "-DCMAKE_TOOLCHAIN_FILE=${CMAKE_TOOLCHAIN_FILE}"
    "-DCMAKE_PREFIX_PATH=${VENDOR_INSTALL_PREFIX}"
    "-DVCPKG_TARGET_TRIPLET=${VCPKG_TARGET_TRIPLET}"
    "-DVCPKG_OVERLAY_TRIPLETS=${VCPKG_OVERLAY_TRIPLETS}"
    "-DVCPKG_MANIFEST_MODE=OFF"
    "-DCMAKE_CXX_STANDARD=17"
    "-DCMAKE_WINDOWS_EXPORT_ALL_SYMBOLS=ON"
    "-DBUILD_SHARED_LIBS=ON"
    "-DBUILD_TESTING=OFF"
    "-DENABLE_TESTING=OFF"
    "-DCMAKE_EXPORT_COMPILE_COMMANDS=ON"
)

# 1. ros_industrial_cmake_boilerplate ─────────────────────────────────────────
# Must come first — every other vendor package calls its macros before project().
ExternalProject_Add(vendor_ricb
    SOURCE_DIR        "${CMAKE_SOURCE_DIR}/vendor/ros_industrial_cmake_boilerplate"
    INSTALL_DIR       "${VENDOR_INSTALL_PREFIX}"
    CMAKE_ARGS        ${_VENDOR_ARGS}
    BUILD_ALWAYS      OFF
    UPDATE_DISCONNECTED ON
)

# 2. opw_kinematics (header-only IK solver) ───────────────────────────────────
ExternalProject_Add(vendor_opw
    SOURCE_DIR        "${CMAKE_SOURCE_DIR}/vendor/opw_kinematics"
    INSTALL_DIR       "${VENDOR_INSTALL_PREFIX}"
    CMAKE_ARGS        ${_VENDOR_ARGS}
    BUILD_ALWAYS      OFF
    UPDATE_DISCONNECTED ON
    DEPENDS vendor_ricb
)

# 3. boost_plugin_loader ──────────────────────────────────────────────────────
ExternalProject_Add(vendor_boost_plugin_loader
    SOURCE_DIR        "${CMAKE_SOURCE_DIR}/vendor/boost_plugin_loader"
    INSTALL_DIR       "${VENDOR_INSTALL_PREFIX}"
    CMAKE_ARGS        ${_VENDOR_ARGS}
    BUILD_ALWAYS      OFF
    UPDATE_DISCONNECTED ON
    DEPENDS vendor_ricb
)

# 5. pinocchio ────────────────────────────────────────────────────────────────
ExternalProject_Add(vendor_pinocchio
    SOURCE_DIR        "${CMAKE_SOURCE_DIR}/vendor/pinocchio"
    INSTALL_DIR       "${VENDOR_INSTALL_PREFIX}"
    CMAKE_ARGS
        ${_VENDOR_ARGS}
        "-DBUILD_PYTHON_INTERFACE=OFF"
        "-DBUILD_EXAMPLES=OFF"
        "-DBUILD_BENCHMARK=OFF"
        "-DBUILD_UTILS=OFF"
        "-DBUILD_WITH_COLLISION_SUPPORT=OFF"
        "-DINSTALL_DOCUMENTATION=OFF"
        "-DGENERATE_PYTHON_STUBS=OFF"
        "-DBUILD_ADVANCED_TESTING=OFF"
    BUILD_ALWAYS      OFF
    UPDATE_DISCONNECTED ON
)

# 4. tesseract ─────────────────────────────────────────────────────────────────
ExternalProject_Add(vendor_tesseract
    SOURCE_DIR        "${CMAKE_SOURCE_DIR}/vendor/tesseract"
    INSTALL_DIR       "${VENDOR_INSTALL_PREFIX}"
    CMAKE_ARGS
        ${_VENDOR_ARGS}
        "-DTESSERACT_ENABLE_TESTING=OFF"
        "-DTESSERACT_ENABLE_BENCHMARKING=OFF"
        "-DTESSERACT_ENABLE_CLANG_TIDY=OFF"
        "-DTESSERACT_PACKAGE=OFF"
    BUILD_ALWAYS      OFF
    UPDATE_DISCONNECTED ON
    DEPENDS vendor_ricb vendor_opw vendor_boost_plugin_loader
)
