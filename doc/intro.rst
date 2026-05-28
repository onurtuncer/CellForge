Introduction and Overview
=========================

Abstract
--------

CellForge is a Windows-first C++ application framework for industrial robotics
and automation. It provides a platform-agnostic core with a typed event system,
an ECS-based workcell model, and pluggable platform backends — currently Qt 6
and Win32/MFC — each driving the same ``Application::Run()`` loop without
coupling the core to any specific GUI toolkit.

This document describes the framework's architecture, key concepts, and design
decisions.

Architecture Overview
---------------------

CellForge is organized in layers:

.. code-block:: text

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

The ``Application`` class owns the main loop and lifecycle hooks. Platform
backends implement ``IApplicationPlatform`` to bridge the native windowing
system to the framework without exposing toolkit-specific types to application
code.

Key Concepts
------------

Application Lifecycle
~~~~~~~~~~~~~~~~~~~~~

Every CellForge application subclasses ``Application`` and overrides three
lifecycle hooks:

``OnInit()``
    Called once after the platform backend is initialized. Create windows,
    register event callbacks, and set up the workcell here.

``OnUpdate()``
    Called every tick — poll sensor data, advance simulations, or check
    window visibility.

``OnShutdown()``
    Called once before the platform backend tears down. Release resources
    acquired in ``OnInit()``.

The factory function ``CreateApplication(int argc, char** argv)`` must be
provided by the application; ``EntryPoint.h`` supplies ``main()`` /
``WinMain()`` and calls it.

Event System
~~~~~~~~~~~~

CellForge uses a typed, dispatcher-based event system that is fully independent
of Qt signals/slots or Win32 message maps:

.. code-block:: cpp

   // Platform widgets translate native events into CellForge events:
   viewport->setEventCallback([this](Event& e) { OnEvent(e); });

   // Application subclasses handle them via EventDispatcher:
   EventDispatcher d(event);
   d.dispatch<KeyPressedEvent>([](KeyPressedEvent& e) {
       CF_CORE_INFO("key {}", static_cast<int>(e.keyCode()));
       return false;
   });

All platform backends translate native events — Qt ``QKeyEvent``,
Win32 ``WM_KEYDOWN``, and so on — into the same ``CellForge::Event``
hierarchy, so application code is identical regardless of the active backend.

ECS-Based Workcell Model
~~~~~~~~~~~~~~~~~~~~~~~~

The workcell layer (``CellForge::workcell``) models a robot cell as an
Entity-Component-System (ECS) world built on the **flecs** library.
Scene entities, robot links, and kinematic relationships are stored as
flecs components and queried efficiently via the flecs query API.
Geometry is represented using **OpenCASCADE** (OCCT) shapes attached to
scene entities.

Platform Backends
~~~~~~~~~~~~~~~~~

Both backends implement three hooks that ``Application::Run()`` calls at
fixed points:

.. code-block:: cpp

   class IApplicationPlatform {
   public:
       virtual void init()       {}   // called once before OnInit()
       virtual void pollEvents() {}   // called every tick (non-blocking pump)
       virtual void shutdown()   {}   // called once after OnShutdown()
   };

.. list-table::
   :header-rows: 1
   :widths: 20 40 40

   * - Backend
     - ``init()``
     - ``pollEvents()``
   * - Qt
     - Creates ``QApplication``, sets ``quitOnLastWindowClosed(false)``
     - ``processEvents(AllEvents, 16 ms)``
   * - MFC
     - Creates ``CWinApp``, calls ``AfxWinInit`` + ``InitApplication``
     - ``PeekMessage`` drain loop, 16 ms sleep when idle

Nomenclature
------------

.. list-table::
   :header-rows: 1
   :widths: 30 70

   * - Term
     - Description
   * - Application
     - Base class for all CellForge applications; owns the main loop
   * - IApplicationPlatform
     - Pure-virtual interface implemented by Qt and MFC backends
   * - Event
     - Base class of the CellForge event hierarchy
   * - EventDispatcher
     - Typed dispatcher that routes an ``Event`` to registered handlers
   * - EventQueue
     - Thread-safe queue used to post events from platform threads
   * - KeyPressedEvent / MouseMovedEvent / …
     - Concrete typed event classes derived from ``Event``
   * - Entity
     - Lightweight identifier in the flecs ECS world
   * - World
     - The flecs ECS world; stores all entities and their components
   * - Workcell
     - A ``World`` populated with robot-cell entities and components
   * - ViewportWidget
     - OCCT-backed 3D viewport widget (Qt backend)
   * - MfcViewportWnd
     - OCCT-backed 3D viewport window (MFC backend)
   * - CF_CORE_INFO / CF_INFO / …
     - spdlog-backed logging macros (core-side and client-side)
   * - CF_PROFILE_SCOPE / CF_PROFILE_FUNCTION
     - Tracy profiling macros

Abbreviations
-------------

.. list-table::
   :header-rows: 1
   :widths: 20 80

   * - Abbreviation
     - Meaning
   * - ECS
     - Entity-Component-System
   * - MFC
     - Microsoft Foundation Classes
   * - OCCT
     - Open CASCADE Technology (OpenCASCADE geometry kernel)
   * - Qt
     - Qt 6 cross-platform application framework
   * - SVA
     - Spatial Vector Algebra (used in Tesseract kinematics)
   * - UUID
     - Universally Unique Identifier (entity key type)
   * - VK
     - Virtual Key (Win32 key-code namespace)
