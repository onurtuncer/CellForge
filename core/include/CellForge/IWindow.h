// ------------------------------------------------------------------------------------
// Project: CellForge
// Copyright (C) 2026, Melina Aero Teknoloji Gelistirme ve Dizayn Burosu A.S., Istanbul
// Author: Onur Tuncer, PhD
//
// SPDX-License-Identifier: LGPL-2.1-only
// License-Filename: LICENSE
// ------------------------------------------------------------------------------------

#pragma once

#include <memory>
#include <string>

namespace CellForge {

class IViewportWidget;

class IWindow
{
public:
    virtual ~IWindow() = default;

    virtual void Show()  = 0;
    virtual void Close() = 0;

    // True while the window is open and visible; false after Close() or user dismissal.
    // Used by ViewerApplication::OnUpdate() to detect when to end the loop.
    virtual bool IsOpen() const = 0;

    virtual void SetTitle(const std::string& title)  = 0;
    virtual void SetSize(int width, int height)       = 0;

    // Embeds a viewport widget as the window's central content area and takes
    // responsibility for forwarding resize events to it.
    virtual void SetCentralViewport(std::shared_ptr<IViewportWidget> viewport) = 0;

    virtual void* NativeHandle() = 0;
};

} // namespace CellForge
