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
#include <vector>

class QApplication;

namespace CellForge {

// Qt implementation of IApplicationPlatform.
// Owns the QApplication object and pumps the Qt event queue each tick via
// QCoreApplication::processEvents() so Application::Run() remains the loop owner.
//
// argc/argv are stored by value so QApplication's internal int& reference
// stays valid for the lifetime of this object.
class QtApplicationPlatform : public IApplicationPlatform {
public:
    QtApplicationPlatform(int argc, char** argv);
    ~QtApplicationPlatform() override;

    // Creates the QApplication — must be called before any Qt widgets are constructed.
    void init()       override;

    // Pumps all pending Qt events (input, paint, timers, signals/slots).
    void pollEvents() override;

    // Destroys the QApplication.
    void shutdown()   override;

private:
    int                          m_Argc;
    std::vector<char*>           m_Argv;
    std::unique_ptr<QApplication> m_App;
};

} // namespace CellForge
