// ------------------------------------------------------------------------------------
// Project: CellForge
// Copyright (C) 2026, Melina Aero Teknoloji Gelistirme ve Dizayn Burosu A.S., Istanbul
// Author: Onur Tuncer, PhD
//
// SPDX-License-Identifier: LGPL-2.1-only
// License-Filename: LICENSE
// ------------------------------------------------------------------------------------

// Qt includes
#include <QApplication>
#include <QHBoxLayout>
#include <QMainWindow>

// CellForge includes
#include "ViewerWidget.h"

using namespace CellForge;

int main(int argc, char** argv)
{
  QApplication app(argc, argv);

  QMainWindow* mainWindow = new QMainWindow();
  mainWindow->setWindowTitle("CellForge Qt-Viewer Example");

  QWidget* centralWidget = new QWidget(mainWindow);
  mainWindow->setCentralWidget(centralWidget);
  QHBoxLayout* hLayout = new QHBoxLayout(centralWidget);
  hLayout->setContentsMargins(6, 6, 6, 6);

  ViewerWidget* viewer = new ViewerWidget(centralWidget);
  hLayout->addWidget(viewer);

  mainWindow->resize(900, 600);
  mainWindow->show();

  return app.exec();

  return 0;
}
