// ------------------------------------------------------------------------------------
// Project: CellForge
// Copyright (C) 2026, Melina Aero Teknoloji Gelistirme ve Dizayn Burosu A.S., Istanbul
// Author: Onur Tuncer, PhD
//
// SPDX-License-Identifier: LGPL-2.1-only
// License-Filename: LICENSE
// ------------------------------------------------------------------------------------

#pragma once

#include <CellForge/IViewPortWidget.h>

namespace CellForge {

// Semantic interface for a viewport widget that also hosts an OCCT 3D view.
// Extends IViewportWidget so a viewer widget can be passed wherever a generic
// viewport widget is accepted (e.g. IWindow::SetCentralViewport).
// Implemented by ViewerWidget (Qt) and MfcViewerWnd (MFC).
class IViewerWidget : public IViewportWidget {};

} // namespace CellForge
