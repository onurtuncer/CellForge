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

class IPlugin
{
public:
    virtual ~IPlugin() = default;

    virtual std::string Name() const = 0;
    virtual std::string Version() const = 0;

    virtual void Initialize() = 0;
    virtual void Shutdown() = 0;
};

}