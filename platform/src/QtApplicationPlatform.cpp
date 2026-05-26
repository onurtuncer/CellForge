// ------------------------------------------------------------------------------------
// Project: CellForge
// Copyright (C) 2026, Melina Aero Teknoloji Gelistirme ve Dizayn Burosu A.S., Istanbul
// Author: Onur Tuncer, PhD
//
// SPDX-License-Identifier: LGPL-2.1-only
// License-Filename: LICENSE
// ------------------------------------------------------------------------------------

#include <CellForge/qt/QtApplicationPlatform.h>

#include <QApplication>
#include <QCoreApplication>
#include <QEventLoop>

namespace CellForge {

QtApplicationPlatform::QtApplicationPlatform(int argc, char** argv)
    : m_Argc(argc)
    , m_Argv(argv, argv + argc)
{}

QtApplicationPlatform::~QtApplicationPlatform() = default;

void QtApplicationPlatform::init()
{
    m_App = std::make_unique<QApplication>(m_Argc, m_Argv.data());

    // We own the quit decision via Application::Close() / OnUpdate().
    // Disable Qt's automatic quit-on-last-window-close so it doesn't post a
    // stray quit event that processEvents() silently swallows.
    m_App->setQuitOnLastWindowClosed(false);
}

void QtApplicationPlatform::pollEvents()
{
    // Cap idle CPU to ~60 fps: process all pending events, then wait up to 16 ms
    // for the next one instead of spinning at 100% when nothing is happening.
    // When OnUpdate() is doing heavy work the wait is skipped because there are
    // always events queued (paint, resize, etc.).
    QCoreApplication::processEvents(QEventLoop::AllEvents, 16);
}

void QtApplicationPlatform::shutdown()
{
    // Qt 6 marks aboutToQuit(QPrivateSignal) so it cannot be emitted externally.
    // The signal only fires inside exec(), which we never call.  Best-effort: drain
    // all pending events so every queued slot that was already connected has run
    // before QApplication is destroyed.  Code that needs guaranteed pre-shutdown
    // teardown should override Application::OnShutdown() instead of using aboutToQuit.
    QCoreApplication::processEvents(QEventLoop::AllEvents);
    m_App.reset();
}

} // namespace CellForge
