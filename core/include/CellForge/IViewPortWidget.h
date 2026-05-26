// ------------------------------------------------------------------------------------
// Project: CellForge
// Copyright (C) 2026, Melina Aero Teknoloji Gelistirme ve Dizayn Burosu A.S., Istanbul
// Author: Onur Tuncer, PhD
//
// SPDX-License-Identifier: LGPL-2.1-only
// License-Filename: LICENSE
// ------------------------------------------------------------------------------------

#pragma once

namespace CellForge
{
    class Event;

    class IViewportWidget
    {
    public:
        virtual ~IViewportWidget() = default;

        virtual void Show() = 0;
        virtual void Hide() = 0;
        virtual void Resize(int width, int height) = 0;
        virtual void Redraw() = 0;

        virtual void* NativeHandle() = 0;

        virtual void SetEventCallback(
            std::function<void(Event&)> callback) = 0;
    };
}