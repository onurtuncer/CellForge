// ------------------------------------------------------------------------------------
// Project: CellForge
// Copyright (C) 2026, Melina Aero Teknoloji Gelistirme ve Dizayn Burosu A.S., Istanbul
// Author: Onur Tuncer, PhD
//
// SPDX-License-Identifier: LGPL-2.1-only
// License-Filename: LICENSE
// ------------------------------------------------------------------------------------

#pragma once

namespace CellForge {

// Platform back-end integration point for Application::Run().
// Each method has a default no-op so back-ends only override what they need.
//
// Lifetime contract (called by Application::Run()):
//   init()        — once, before OnInit()
//   pollEvents()  — every tick, before ProcessEvents() / OnUpdate()
//   shutdown()    — once, after OnShutdown()
class IApplicationPlatform {
public:
    virtual ~IApplicationPlatform() = default;

    virtual void init()       {}
    virtual void pollEvents() {}
    virtual void shutdown()   {}
};

} // namespace CellForge
