// ------------------------------------------------------------------------------------
// Project: CellForge
// Copyright (C) 2026, Melina Aero Teknoloji Gelistirme ve Dizayn Burosu A.S., Istanbul
// Author: Onur Tuncer, PhD
//
// SPDX-License-Identifier: LGPL-2.1-only
// License-Filename: LICENSE
// ------------------------------------------------------------------------------------

#pragma once

#include <functional>

namespace CellForge {

class Event;

// Platform-agnostic interface for a viewport widget: an embeddable surface
// that forwards input/window events to a CellForge callback.
// Implemented by ViewportWidget (Qt) and MfcViewportWnd (MFC).
class IViewportWidget
{
public:
    virtual ~IViewportWidget() = default;

    // Returns the underlying native widget handle (QWidget* on Qt, HWND on MFC)
    // so the owning window can embed it as a child.
    virtual void* NativeHandle() = 0;

    // Resize the widget to the given pixel dimensions.
    // Called by the parent window's resize handler.
    virtual void resize(int cx, int cy) = 0;

    virtual void setEventCallback(std::function<void(Event&)> callback) = 0;
};

} // namespace CellForge
