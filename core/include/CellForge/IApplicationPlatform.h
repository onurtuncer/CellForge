// ------------------------------------------------------------------------------------
// Project: CellForge
// Copyright (C) 2026, Melina Aero Teknoloji Gelistirme ve Dizayn Burosu A.S., Istanbul
// Author: Onur Tuncer, PhD
//
// SPDX-License-Identifier: LGPL-2.1-only
// License-Filename: LICENSE
// ------------------------------------------------------------------------------------

#pragma once

namespace CellForge{

    class IApplicationPlatform
    {
    public:
        virtual ~IApplicationPlatform() = default;

        virtual void Initialize(int argc, char** argv) = 0;
        virtual int Run() = 0;
        virtual void Shutdown() = 0;

        virtual std::shared_ptr<IWindow> CreateMainWindow(
            const std::string& title,
            int width,
            int height) = 0;

        virtual std::shared_ptr<IViewportWidget> CreateViewportWidget() = 0;

        virtual std::shared_ptr<IMenuBar> CreateMenuBar(
            std::shared_ptr<IWindow> window) = 0;

        virtual std::shared_ptr<IDockingSystem> CreateDockingSystem(
            std::shared_ptr<IWindow> window) = 0;
    };
} 



}

