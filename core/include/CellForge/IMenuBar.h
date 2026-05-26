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

        class IMenuBar
    {
    public:
        virtual ~IMenuBar() = default;

        virtual void AddMenu(const std::string& name) = 0;

        virtual void AddAction(
            const std::string& menu,
            const std::string& actionName,
            std::function<void()> callback) = 0;
    };

}