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
}

void QtApplicationPlatform::pollEvents()
{
    QCoreApplication::processEvents(QEventLoop::AllEvents);
}

void QtApplicationPlatform::shutdown()
{
    m_App.reset();
}

} // namespace CellForge
