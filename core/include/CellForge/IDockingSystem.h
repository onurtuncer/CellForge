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
    
        class IDockingSystem
    {
    public:
        virtual ~IDockingSystem() = default;

        virtual void AddDockPanel(
            const std::string& name,
            void* nativeWidget,
            const std::string& area) = 0;

        virtual void RemoveDockPanel(const std::string& name) = 0;

        virtual void ShowDockPanel(const std::string& name) = 0;
        virtual void HideDockPanel(const std::string& name) = 0;
    };
}