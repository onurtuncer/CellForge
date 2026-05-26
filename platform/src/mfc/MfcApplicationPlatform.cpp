// ------------------------------------------------------------------------------------
// Project: CellForge
// Copyright (C) 2026, Melina Aero Teknoloji Gelistirme ve Dizayn Burosu A.S., Istanbul
// Author: Onur Tuncer, PhD
//
// SPDX-License-Identifier: LGPL-2.1-only
// License-Filename: LICENSE
// ------------------------------------------------------------------------------------

#include <CellForge/mfc/MfcApplicationPlatform.h>

#ifndef _AFXDLL
#define _AFXDLL
#endif
#include <afxwin.h>

namespace CellForge {

MfcApplicationPlatform::~MfcApplicationPlatform() = default;

void MfcApplicationPlatform::init()
{
    // CWinApp constructor registers this object as the MFC application singleton
    // (AfxSetCurrentApp) — it must exist before AfxWinInit is called.
    m_WinApp = std::make_unique<CWinApp>();

    HINSTANCE hInst = ::GetModuleHandleW(nullptr);
    if (!AfxWinInit(hInst, nullptr, ::GetCommandLineW(), SW_SHOW))
        AfxThrowMemoryException(); // MFC init failed — fatal

    // InitApplication registers common window classes and sets up MFC's
    // module state so CWnd::Create / CreateEx work correctly.
    m_WinApp->InitApplication();
}

void MfcApplicationPlatform::pollEvents()
{
    // Non-blocking drain: process every pending message before returning.
    MSG msg;
    bool hadMessages = false;
    while (::PeekMessageW(&msg, nullptr, 0, 0, PM_REMOVE))
    {
        ::TranslateMessage(&msg);
        ::DispatchMessageW(&msg);
        hadMessages = true;
    }

    // Cap idle CPU to ~60 fps: if the queue was already empty, sleep 16 ms.
    // When the app is doing real work (rendering, simulation) there will always
    // be paint/update messages queued and this branch is never taken.
    if (!hadMessages)
        ::Sleep(16);
}

void MfcApplicationPlatform::shutdown()
{
    // Drain any remaining messages (e.g. WM_DESTROY posted by window teardown).
    MSG msg;
    while (::PeekMessageW(&msg, nullptr, 0, 0, PM_REMOVE))
    {
        ::TranslateMessage(&msg);
        ::DispatchMessageW(&msg);
    }
    m_WinApp.reset();
}

} // namespace CellForge
