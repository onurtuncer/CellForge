// ------------------------------------------------------------------------------------
// Project: CellForge
// Copyright (C) 2026, Melina Aero Teknoloji Gelistirme ve Dizayn Burosu A.S., Istanbul
// Author: Onur Tuncer, PhD
//
// SPDX-License-Identifier: LGPL-2.1-only
// License-Filename: LICENSE
// ------------------------------------------------------------------------------------

#pragma once

//TODO [Onur] check Hazel style does core is namespaced?
namespace CellForge{

     class IWindow
    {
    public:
        virtual ~IWindow() = default;

        virtual void Show() = 0;
        virtual void Close() = 0;

        virtual void SetTitle(const std::string& title) = 0;
        virtual void SetSize(int width, int height) = 0;

        virtual void SetCentralViewport(
            std::shared_ptr<IViewportWidget> viewport) = 0;

        virtual void* NativeHandle() = 0;
    };

} // namespace CellForge


