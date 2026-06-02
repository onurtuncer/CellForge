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
# Use Ninja so each TU gets its own cl.exe invocation. MSBuild batches all 30
# template-heavy TUs into one process which causes multi-hour optimizer hangs.
# MSVC /wd4716 /EHsc are Windows-only; GCC treats them as file paths on Linux.
if(WIN32)
    # Use clang-cl for pinocchio: MSVC's optimizer hangs for hours on
    # admm-solver.cpp's deeply-instantiated Eigen templates; clang handles
    # the same code in seconds while still producing MSVC-ABI-compatible
    # objects that link cleanly against the rest of the MSVC build.
    set(_clangcl
        "C:/Program Files/Microsoft Visual Studio/18/Community/VC/Tools/Llvm/x64/bin/clang-cl.exe")
    set(_pinocchio_cxx_flags
        "-DCMAKE_C_COMPILER=${_clangcl}"
        "-DCMAKE_CXX_COMPILER=${_clangcl}"
        "-DCMAKE_CXX_FLAGS=/wd4716 /EHsc"
    )
else()
    set(_pinocchio_cxx_flags "")
endif()
ExternalProject_Add(vendor_pinocchio
    SOURCE_DIR        "${CMAKE_SOURCE_DIR}/vendor/pinocchio"
    INSTALL_DIR       "${VENDOR_INSTALL_PREFIX}"
    CMAKE_GENERATOR   "Ninja"
    CMAKE_ARGS
        ${_VENDOR_ARGS}
        # pinocchio 4.x uses jrl-cmakemodules which redefines find_package,
        # add_executable, and add_library.  vcpkg.cmake overrides the same three
        # commands; jrl's later redefinitions then alias _<cmd> back to vcpkg's
        # own function/macro, causing 1000-level infinite recursion in all three.
        # Fix: supply a custom toolchain (last -D wins) that sets up the vcpkg
        # prefix path directly without including vcpkg.cmake at all.
        "-DCMAKE_TOOLCHAIN_FILE=${CMAKE_SOURCE_DIR}/cmake/pinocchio_toolchain.cmake"
        ${_pinocchio_cxx_flags}
        "-DBUILD_SHARED_LIBS=OFF"
        "-DBUILD_VISUALIZERS=OFF"
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
