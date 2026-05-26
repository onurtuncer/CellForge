// ------------------------------------------------------------------------------------
// Project: CellForge
// Copyright (C) 2026, Melina Aero Teknoloji Gelistirme ve Dizayn Burosu A.S., Istanbul
// Author: Onur Tuncer, PhD
//
// SPDX-License-Identifier: LGPL-2.1-only
// License-Filename: LICENSE
// ------------------------------------------------------------------------------------

// Demonstrates CellForge Application owning the event loop via MfcApplicationPlatform:
//   - Application::Run() drives the loop; CWinApp::Run() is never called.
//   - MfcViewportWnd translates Win32 messages into typed CellForge events.
//   - Events are routed through Application::OnEvent() and logged to the console.

#include <CellForge/Application.h>
#include <CellForge/EntryPoint.h>
#include <CellForge/Log.h>
#include <CellForge/Event/EventDispatcher.h>
#include <CellForge/Event/KeyboardEvents.h>
#include <CellForge/Event/MouseEvents.h>
#include <CellForge/Event/ApplicationEvent.h>
#include <CellForge/mfc/MfcApplicationPlatform.h>
#include <CellForge/mfc/MfcViewportWnd.h>

#ifndef _AFXDLL
#define _AFXDLL
#endif
#include <afxwin.h>

namespace CellForge {

// ── Main window ───────────────────────────────────────────────────────────────
// Minimal top-level CWnd. Hosts MfcViewportWnd as its only child and forwards
// WM_CLOSE to Application::Close() so the loop exits cleanly.

class MfcMainWnd : public CWnd {
    DECLARE_MESSAGE_MAP()
public:
    bool create(const TCHAR* title, int w, int h)
    {
        LPCTSTR cls = AfxRegisterWndClass(
            CS_HREDRAW | CS_VREDRAW,
            ::LoadCursor(nullptr, IDC_ARROW),
            reinterpret_cast<HBRUSH>(COLOR_WINDOW + 1),
            nullptr);

        return CWnd::CreateEx(0, cls, title,
            WS_OVERLAPPEDWINDOW,
            CW_USEDEFAULT, CW_USEDEFAULT, w, h,
            nullptr, nullptr) == TRUE;
    }

protected:
    afx_msg void OnClose()
    {
        Application::Get().Close();
        DestroyWindow();
    }

    afx_msg void OnSize(UINT nType, int cx, int cy)
    {
        CWnd::OnSize(nType, cx, cy);
        if (m_Viewport && m_Viewport->m_hWnd)
            m_Viewport->resize(cx, cy);
    }

public:
    MfcViewportWnd* m_Viewport = nullptr;
};

BEGIN_MESSAGE_MAP(MfcMainWnd, CWnd)
    ON_WM_CLOSE()
    ON_WM_SIZE()
END_MESSAGE_MAP()

// ── Application subclass ──────────────────────────────────────────────────────

class MfcEventLogApp : public Application {
public:
    MfcEventLogApp()
        : Application({"MFC Event Log Demo"},
                      std::make_unique<MfcApplicationPlatform>())
    {}

    void OnInit() override
    {
        m_MainWnd = std::make_unique<MfcMainWnd>();
        if (!m_MainWnd->create(_T("CellForge \x2014 MFC Event Log Demo"), 800, 600))
        {
            CF_CORE_ERROR("Failed to create main window.");
            Close();
            return;
        }

        // Viewport fills the whole client area.
        CRect clientRect;
        m_MainWnd->GetClientRect(&clientRect);

        m_Viewport = std::make_unique<MfcViewportWnd>();
        if (!m_Viewport->create(m_MainWnd.get(), clientRect))
        {
            CF_CORE_ERROR("Failed to create viewport window.");
            Close();
            return;
        }

        m_MainWnd->m_Viewport = m_Viewport.get();
        m_Viewport->setEventCallback([this](Event& e) { OnEvent(e); });

        m_MainWnd->ShowWindow(SW_SHOW);
        m_MainWnd->UpdateWindow();
        m_Viewport->SetFocus();

        // Register logging callback.
        AddEventCallback([](Event& event) {
            EventDispatcher d(event);

            d.dispatch<KeyPressedEvent>([](KeyPressedEvent& e) {
                CF_CORE_INFO("[KeyPressed]    keyCode={} repeat={}",
                    static_cast<int>(e.keyCode()), e.repeat());
                return false;
            });
            d.dispatch<KeyReleasedEvent>([](KeyReleasedEvent& e) {
                CF_CORE_INFO("[KeyReleased]   keyCode={}",
                    static_cast<int>(e.keyCode()));
                return false;
            });
            d.dispatch<MouseButtonPressedEvent>([](MouseButtonPressedEvent& e) {
                CF_CORE_INFO("[MousePressed]  button={} x={:.1f} y={:.1f}",
                    static_cast<int>(e.button()), e.x(), e.y());
                return false;
            });
            d.dispatch<MouseButtonReleasedEvent>([](MouseButtonReleasedEvent& e) {
                CF_CORE_INFO("[MouseReleased] button={} x={:.1f} y={:.1f}",
                    static_cast<int>(e.button()), e.x(), e.y());
                return false;
            });
            d.dispatch<MouseMovedEvent>([](MouseMovedEvent& e) {
                CF_CORE_TRACE("[MouseMoved]    x={:.1f} y={:.1f}", e.x(), e.y());
                return false;
            });
            d.dispatch<MouseScrolledEvent>([](MouseScrolledEvent& e) {
                CF_CORE_INFO("[MouseScrolled] dy={:.2f}", e.yOffset());
                return false;
            });
            d.dispatch<WindowResizeEvent>([](WindowResizeEvent& e) {
                CF_CORE_INFO("[WindowResize]  {}x{}", e.width(), e.height());
                return false;
            });
        });

        CF_CORE_INFO("MFC application running — interact with the viewport.");
    }

    void OnUpdate() override
    {
        // MfcMainWnd::OnClose calls Application::Close() + DestroyWindow(), so
        // once the HWND is gone the loop should stop.
        if (m_MainWnd && !m_MainWnd->m_hWnd)
            Close();
    }

    void OnShutdown() override
    {
        CF_CORE_INFO("Application shutting down.");
        m_Viewport.reset();
        m_MainWnd.reset();
    }

private:
    std::unique_ptr<MfcMainWnd>      m_MainWnd;
    std::unique_ptr<MfcViewportWnd>  m_Viewport;
};

Application* CreateApplication(int /*argc*/, char** /*argv*/)
{
    return new MfcEventLogApp();
}

} // namespace CellForge
