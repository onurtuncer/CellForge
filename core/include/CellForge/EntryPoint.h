// ------------------------------------------------------------------------------------
// Project: CellForge
// Copyright (C) 2026, Melina Aero Teknoloji Gelistirme ve Dizayn Burosu A.S., Istanbul
// Author: Onur Tuncer, PhD
//
// SPDX-License-Identifier: LGPL-2.1-only
// License-Filename: LICENSE
// ------------------------------------------------------------------------------------

#pragma once

#include "CellForge/Application.h"
#include "CellForge/Assert.h"

namespace CellForge {

    int Main(int argc, char** argv)
    {
        Application* app = CreateApplication(argc, argv);
        CF_CORE_ASSERT(app, "Client Application is null!");
        app->Run();
        delete app;
        return 0;
    }

} // namespace CellForge

#if defined(CF_DIST) && defined(CF_PLATFORM_WINDOWS)

int APIENTRY WinMain(HINSTANCE hInst, HINSTANCE hInstPrev, PSTR cmdline, int cmdshow)
{
    return CellForge::Main(__argc, __argv);
}

#else

int main(int argc, char** argv)
{
    return CellForge::Main(argc, argv);
}

#endif
