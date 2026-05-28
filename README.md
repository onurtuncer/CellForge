# CellForge

[![Windows](https://github.com/onurtuncer/CellForge/actions/workflows/windows.yml/badge.svg)](https://github.com/onurtuncer/CellForge/actions/workflows/windows.yml)
[![Linux](https://github.com/onurtuncer/CellForge/actions/workflows/linux.yml/badge.svg)](https://github.com/onurtuncer/CellForge/actions/workflows/linux.yml)

A Windows-first C++ application framework for industrial robotics and automation.  
CellForge provides a platform-agnostic core with typed events, an ECS-based workcell model, and pluggable platform backends — currently Qt 6 and Win32/MFC — that each drive `Application::Run()` without coupling the core to any GUI toolkit.

---

## Architecture

```
┌─────────────────────────────────────────────────────────────┐
│                        Application                          │
│  Run() → platform->init()                                   │
│          OnInit()                                           │
│          loop { platform->pollEvents()                      │
│                 ProcessEvents()   ← CellForge event queue   │
│                 OnUpdate() }                                │
│          OnShutdown()                                       │
│          platform->shutdown()                               │
└──────────────────────┬──────────────────────────────────────┘
                       │ IApplicationPlatform
          ┌────────────┴────────────┐
          │                         │
 QtApplicationPlatform     MfcApplicationPlatform
 QCoreApplication::         PeekMessage /
 processEvents()            TranslateMessage /
                            DispatchMessage
```

### Components

| Directory | CMake target | Description |
|---|---|---|
| `core/` | `CellForge::core` | Application lifecycle, typed event system, ECS (flecs), logging (spdlog), Tracy profiling, memory tracking |
| `workcell/` | `CellForge::workcell` | ECS-based robot cell model — entities, components, scene loading/querying (flecs + OpenCASCADE) |
| `gui/` | `CellForge::viewer` | Interactive 3D viewer widget (Qt 6 + OpenCASCADE OCCT) |
| `platform/` | `CellForge::platform_qt` | Qt 6 platform backend — `QtApplicationPlatform`, `ViewportWidget`, key/mouse event mapper |
| `platform/` | `CellForge::platform_mfc` | Win32 / MFC platform backend — `MfcApplicationPlatform`, `MfcViewportWnd`, VK key mapper *(requires VS MFC component)* |
| `vendor/` | — | Source-built dependencies: Tesseract, opw_kinematics, boost_plugin_loader, RICB |

### Event system

CellForge uses a typed, dispatcher-based event system that is entirely independent of Qt signals/slots or Win32 message maps:

```cpp
// Platform widgets translate native events and push them into CellForge:
viewport->setEventCallback([this](Event& e) { OnEvent(e); });

// Application subclass handles them via EventDispatcher:
EventDispatcher d(event);
d.dispatch<KeyPressedEvent>([](KeyPressedEvent& e) {
    CF_CORE_INFO("key {}", static_cast<int>(e.keyCode()));
    return false;
});
```

All platform backends translate their native events (Qt `QKeyEvent`, Win32 `WM_KEYDOWN`, …) into the same `CellForge::Event` hierarchy, so application code is identical regardless of the backend in use.

### Platform backends

Both backends implement three hooks that `Application::Run()` calls at fixed points:

```cpp
class IApplicationPlatform {
public:
    virtual void init()       {}   // called once before OnInit()
    virtual void pollEvents() {}   // called every tick (non-blocking pump)
    virtual void shutdown()   {}   // called once after OnShutdown()
};
```

| Backend | `init()` | `pollEvents()` | `shutdown()` |
|---|---|---|---|
| Qt | creates `QApplication`, sets `quitOnLastWindowClosed(false)` | `processEvents(AllEvents, 16 ms)` | drains queue, destroys `QApplication` |
| MFC | creates `CWinApp`, calls `AfxWinInit` + `InitApplication` | `PeekMessage` drain loop, 16 ms sleep when idle | drain remaining messages, destroys `CWinApp` |

---

## Repository layout

```
CellForge/
├── CMakeLists.txt              # Top-level superbuild
├── CMakePresets.json           # Ready-to-use configure/build presets
├── cmake/
│   ├── CellForgeComponents.cmake   # Per-component build options
│   ├── VendorDependencies.cmake    # ExternalProject rules for vendor/
│   ├── cellforge_macros.cmake      # Shared CMake helpers
│   └── triplets/
│       └── x64-windows-release.cmake  # vcpkg triplet (dynamic, release only)
├── core/                       # CellForge::core — platform-agnostic
│   ├── include/CellForge/
│   │   ├── Application.h / .cpp
│   │   ├── IApplicationPlatform.h
│   │   ├── EntryPoint.h
│   │   ├── Event/              # Event, EventDispatcher, EventQueue, typed events
│   │   ├── Input/              # KeyCodes, MouseButton
│   │   ├── ecs/                # UUID, Entity, World, Relationship
│   │   ├── Log.h               # spdlog wrapper (CF_CORE_INFO / CF_INFO / …)
│   │   └── Debug/Profiler.h    # Tracy integration macros
│   └── src/
├── workcell/                   # CellForge::workcell — ECS robot cell
├── gui/                        # CellForge::viewer — Qt6 + OCCT 3D viewer
├── platform/                   # Platform backends
│   ├── include/CellForge/
│   │   ├── qt/                 # Qt headers (ViewportWidget, QtApplicationPlatform, QtEventMapper)
│   │   └── mfc/                # MFC headers (MfcViewportWnd, MfcApplicationPlatform, MfcKeyMapper)
│   ├── src/
│   │   ├── qt/                 # Qt sources
│   │   └── mfc/                # MFC sources
│   ├── mfc/CMakeLists.txt      # Isolated subdir — sets CMAKE_MFC_FLAG=2
│   └── examples/
│       ├── qt/                 # Qt event log demo (example_event_log)
│       └── mfc/                # MFC event log demo (example_mfc_event_log)
├── tests/
└── vendor/                     # Git submodules
    ├── ros_industrial_cmake_boilerplate/
    ├── opw_kinematics/
    ├── boost_plugin_loader/
    └── tesseract/
```

---

## Prerequisites

| Requirement | Version | Notes |
|---|---|---|
| Windows | 10 / 11 x64 | |
| Visual Studio | 2022 (v143) or 2026 (v180) | **Desktop development with C++** workload |
| CMake | ≥ 3.18 | |
| vcpkg | latest | installed at `C:\vcpkg` (classic mode) |
| Qt 6 | 6.x | via vcpkg |
| MFC | — | *Optional* — "C++ MFC for latest build tools" VS Installer component, required only for `platform_mfc` |

### Install vcpkg (classic mode)

```powershell
git clone https://github.com/microsoft/vcpkg.git C:\vcpkg
C:\vcpkg\bootstrap-vcpkg.bat
[System.Environment]::SetEnvironmentVariable("VCPKG_ROOT", "C:\vcpkg", "User")
```

> **Note:** CellForge uses vcpkg in **classic mode** — packages are installed to `C:\vcpkg\installed\`, not into the project directory. Do not use `vcpkg integrate install` or manifest mode.

Install the required packages:

```powershell
vcpkg install spdlog flecs tracy qt6-base opencascade yaml-cpp `
              eigen3 bullet3 fcl assimp boost-dll gtest `
              --triplet x64-windows-release --classic
```

### Clone

```powershell
git clone --recurse-submodules https://github.com/onurtuncer/CellForge.git
cd CellForge
```

If you already cloned without submodules:

```powershell
git submodule update --init --recursive
```

---

## Build

On first configure CMake automatically bootstraps `ros_industrial_cmake_boilerplate` from `vendor/` — this takes a few extra seconds once only.

### Visual Studio (recommended)

```powershell
cmake --preset windows-vs2022-x64-release
cmake --build --preset windows-vs2022-x64-release
```

### Ninja (faster incremental builds)

Run from a **Developer PowerShell for VS** so MSVC is on `PATH`:

```powershell
cmake --preset windows-ninja-x64-release
cmake --build --preset windows-ninja-x64-release
```

Debug variant:

```powershell
cmake --preset windows-ninja-x64-debug
cmake --build --preset windows-ninja-x64-debug
```

### Component options

| CMake option | Default | Description |
|---|---|---|
| `CELLFORGE_BUILD_CORE` | `ON` | Core library |
| `CELLFORGE_BUILD_WORKCELL` | `ON` | ECS workcell library |
| `CELLFORGE_BUILD_VIEWER` | `ON` | Qt6 + OCCT viewer library |
| `CELLFORGE_BUILD_PLATFORM_QT` | `ON` | Qt platform backend |
| `CELLFORGE_PLATFORM_QT_BUILD_EXAMPLES` | `OFF` | Qt event log example |
| `CELLFORGE_PLATFORM_MFC_BUILD_EXAMPLES` | `OFF` | MFC event log example |

---

## Writing an application

Subclass `Application`, pass an `IApplicationPlatform` to the constructor, and implement `CreateApplication`:

```cpp
#include <CellForge/Application.h>
#include <CellForge/EntryPoint.h>
#include <CellForge/qt/QtApplicationPlatform.h>
#include <CellForge/qt/widgets/ViewPortWidget.h>

namespace CellForge {

class MyApp : public Application {
public:
    MyApp(int argc, char** argv)
        : Application({"My App"}, std::make_unique<QtApplicationPlatform>(argc, argv))
    {}

    void OnInit() override
    {
        // create widgets, register event callbacks …
    }

    void OnUpdate() override
    {
        // called every tick — check window visibility, update simulation, etc.
        if (m_Window && !m_Window->isVisible())
            Close();
    }

    void OnShutdown() override { /* cleanup */ }

private:
    QMainWindow* m_Window = nullptr;
};

Application* CreateApplication(int argc, char** argv)
{
    return new MyApp(argc, argv);
}

} // namespace CellForge
```

`EntryPoint.h` provides `main()` / `WinMain()` and calls `CreateApplication` → `Run()` → `delete`.

---

## vcpkg dependencies

| Package | Used by |
|---|---|
| `spdlog` | Logging throughout |
| `flecs` | ECS (core, workcell) |
| `tracy` | Frame / scope profiling |
| `yaml-cpp` | Application settings |
| `qt6-base` | Qt platform backend, viewer |
| `opencascade` | 3D geometry kernel (workcell, viewer) |
| `eigen3` | Math, kinematics |
| `bullet3` | Collision detection |
| `fcl` | Flexible collision library |
| `assimp` | Mesh loading |
| `boost-dll` | Plugin loader |
| `gtest` | Unit tests |

All packages use the custom triplet `cmake/triplets/x64-windows-release.cmake` — dynamic-release libraries, matching the Tesseract Windows CI configuration.

---

## Vendored submodules

| Submodule | Source | Version |
|---|---|---|
| `vendor/ros_industrial_cmake_boilerplate` | ros-industrial/ros_industrial_cmake_boilerplate | 0.7.4 |
| `vendor/opw_kinematics` | Jmeyer1292/opw_kinematics | 0.5.3 |
| `vendor/boost_plugin_loader` | tesseract-robotics/boost_plugin_loader | 0.4.3 |
| `vendor/tesseract` | tesseract-robotics/tesseract | 0.34.x |

All vendor packages are built via CMake `ExternalProject` at build time and staged into `<build_dir>/vendor_install/`.

---

## License

[LGPL-2.1-only](LICENSE) — © 2026 Melina Aero Teknoloji Gelistirme ve Dizayn Burosu A.S., Istanbul
