# CellForge

[![Windows](https://github.com/onurtuncer/CellForge/actions/workflows/windows.yml/badge.svg)](https://github.com/onurtuncer/CellForge/actions/workflows/windows.yml)

A Windows-first robotics application built on [Tesseract Robotics](https://github.com/tesseract-robotics/tesseract) — an open-source motion planning and collision checking framework.

---

## Repository layout

```
CellForge/
├── CMakeLists.txt              # Top-level superbuild
├── CMakePresets.json           # Ready-to-use configure/build presets
├── vcpkg.json                  # vcpkg dependency manifest
├── cmake/
│   └── triplets/
│       └── x64-windows-release.cmake   # Custom vcpkg triplet (dynamic, release)
└── vendor/                     # Source-built dependencies (git submodules)
    ├── ros_industrial_cmake_boilerplate/   # CMake utility macros (required by all others)
    ├── opw_kinematics/                     # Header-only analytical IK solver
    ├── boost_plugin_loader/                # Boost-based plugin loading utilities
    └── tesseract/                          # Core robotics framework
        ├── common/             # Shared utilities and types
        ├── geometry/           # Geometric primitives
        ├── scene_graph/        # Robot scene representation
        ├── state_solver/       # Joint state solving
        ├── collision/          # Collision checking (Bullet, FCL)
        ├── srdf/               # Semantic robot description format
        ├── urdf/               # URDF parser
        ├── kinematics/         # Kinematic solvers (OPW, KDL, …)
        └── environment/        # Planning environment
```

---

## Prerequisites

| Requirement | Version | Notes |
|---|---|---|
| Windows | 10 / 11 x64 | |
| MSVC | 2019 (v142) or 2022 (v143) | C++ workload required |
| CMake | ≥ 3.18 | [cmake.org](https://cmake.org/download/) |
| Ninja | any | Optional — needed for the Ninja presets |
| vcpkg | latest | See setup below |

### Install vcpkg

```powershell
git clone https://github.com/microsoft/vcpkg.git C:\vcpkg
C:\vcpkg\bootstrap-vcpkg.bat
[System.Environment]::SetEnvironmentVariable("VCPKG_ROOT", "C:\vcpkg", "User")
```

Restart your shell so `VCPKG_ROOT` is visible to CMake.

### Clone this repository

```powershell
git clone --recurse-submodules https://github.com/onurtuncer/CellForge.git
cd CellForge
```

If you already cloned without `--recurse-submodules`:

```powershell
git submodule update --init --recursive
```

---

## Build

The build has two stages that happen automatically in sequence:

1. **vcpkg** installs all binary dependencies declared in `vcpkg.json` (Eigen, Bullet3, FCL, Boost, YAML-cpp, Assimp, PCL, …).
2. **CMake superbuild** compiles and stages the vendored source packages in dependency order:

   ```
   ros_industrial_cmake_boilerplate
           ↓              ↓
     opw_kinematics   boost_plugin_loader
           ↓              ↓
              tesseract
   ```

   All vendor packages install into `<build_dir>/vendor_install/`.

### Option A — Visual Studio 2022 (recommended)

```powershell
cmake --preset windows-vs2022-x64-release
cmake --build --preset windows-vs2022-x64-release
```

Opens / builds a full VS solution. The preset handles generator selection and vcpkg toolchain automatically.

### Option B — Ninja (faster incremental builds)

Run from a **Developer PowerShell for VS** so the MSVC compiler is on `PATH`:

```powershell
cmake --preset windows-ninja-x64-release
cmake --build --preset windows-ninja-x64-release
```

A Debug variant is also available:

```powershell
cmake --preset windows-ninja-x64-debug
cmake --build --preset windows-ninja-x64-debug
```

### Option C — Manual configure (no presets)

```powershell
cmake -B build -G "Visual Studio 17 2022" -A x64 `
      -DCMAKE_TOOLCHAIN_FILE="$env:VCPKG_ROOT/scripts/buildsystems/vcpkg.cmake" `
      -DVCPKG_TARGET_TRIPLET=x64-windows-release `
      -DVCPKG_OVERLAY_TRIPLETS="cmake/triplets" `
      -DCMAKE_BUILD_TYPE=Release

cmake --build build --config Release
```

---

## vcpkg dependencies

All binary dependencies are listed in [`vcpkg.json`](vcpkg.json) and installed automatically on first configure.
Key packages:

| Package | Used by |
|---|---|
| `eigen3` | Math, kinematics throughout |
| `bullet3[multithreading,double-precision,rtti]` | Collision detection |
| `fcl` | Flexible collision library |
| `octomap` | Voxel-based environment representation |
| `assimp` | Mesh loading |
| `urdfdom` | URDF parsing |
| `orocos-kdl` | KDL kinematic solver backend |
| `pcl` | Point cloud support |
| `console-bridge` | Logging |
| `yaml-cpp` | Configuration / SRDF parsing |
| `boost-dll`, `boost-filesystem`, … | Plugin loader, file I/O |
| `cereal` | Serialization |
| `opencascade` | 3D geometry kernel (OCCT) — CAD/CAM primitives, BRep modelling |
| `qtbase` | Qt 6 Core / GUI / Widgets — UI layer |
| `gtest` | Unit testing |

The custom triplet `cmake/triplets/x64-windows-release.cmake` builds all vcpkg packages as **dynamic-release** libraries, matching the Tesseract Windows CI configuration.

---

## Submodules

| Submodule | Remote | Tag |
|---|---|---|
| `vendor/ros_industrial_cmake_boilerplate` | ros-industrial/ros_industrial_cmake_boilerplate | 0.7.4 |
| `vendor/opw_kinematics` | Jmeyer1292/opw_kinematics | 0.5.3 |
| `vendor/tesseract` | tesseract-robotics/tesseract | 0.34.x |

`vendor/boost_plugin_loader` (tesseract-robotics/boost_plugin_loader 0.4.3) is included as a plain directory.

---

## Adding CellForge application code

Once the vendor packages are built into `<build_dir>/vendor_install/`, add your own targets at the bottom of `CMakeLists.txt`:

```cmake
list(APPEND CMAKE_PREFIX_PATH "${VENDOR_INSTALL_PREFIX}")
find_package(tesseract_common      REQUIRED)
find_package(tesseract_collision   REQUIRED)
find_package(tesseract_kinematics  REQUIRED)
find_package(tesseract_environment REQUIRED)

add_subdirectory(src)
```

---

## License

See [LICENSE](LICENSE).
