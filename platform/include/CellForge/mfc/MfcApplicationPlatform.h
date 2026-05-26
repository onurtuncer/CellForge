// ------------------------------------------------------------------------------------
// Project: CellForge
// Copyright (C) 2026, Melina Aero Teknoloji Gelistirme ve Dizayn Burosu A.S., Istanbul
// Author: Onur Tuncer, PhD
//
// SPDX-License-Identifier: LGPL-2.1-only
// License-Filename: LICENSE
// ------------------------------------------------------------------------------------

#pragma once

#include <CellForge/IApplicationPlatform.h>

#include <memory>

class CWinApp;

namespace CellForge {

// MFC implementation of IApplicationPlatform.
// Owns the CWinApp object (MFC's equivalent of QApplication) and pumps the
// Win32 message queue each tick via PeekMessage so Application::Run() remains
// the single blocking loop — CWinApp::Run() / exec() is never called.
class MfcApplicationPlatform : public IApplicationPlatform {
public:
    MfcApplicationPlatform() = default;
    ~MfcApplicationPlatform() override;

    // Creates CWinApp, calls AfxWinInit to initialise MFC internals.
    void init()       override;

    // Non-blocking Win32 message pump — drains all pending messages.
    // Capped to ~60 fps idle via a 16 ms sleep when the queue is empty.
    void pollEvents() override;

    // Destroys CWinApp.
    void shutdown()   override;

private:
    std::unique_ptr<CWinApp> m_WinApp;
};

} // namespace CellForge
