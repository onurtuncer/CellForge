// ------------------------------------------------------------------------------------
// Project: CellForge
// Copyright (C) 2026, Melina Aero Teknoloji Gelistirme ve Dizayn Burosu A.S., Istanbul
// Author: Onur Tuncer, PhD
//
// SPDX-License-Identifier: LGPL-2.1-only
// License-Filename: LICENSE
// ------------------------------------------------------------------------------------

#pragma once 

#include <vector>

namespace CellForge{

class PluginManager
{
public:

    void LoadPlugins();

    void UnloadPlugins();

private:

    struct LoadedPlugin
    {
        HMODULE Module;
        IPlugin* Instance;
    };

    std::vector<LoadedPlugin> m_Plugins;
};

}

