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
#include "CellForge/Log.h"

#include <exception>

#if defined(CF_PLATFORM_WINDOWS)
#include <excpt.h>   // EXCEPTION_EXECUTE_HANDLER
#endif

namespace CellForge {

    // C++ exception boundary — catches std::exception and unknown throwables.
    static int RunApplication(int argc, char** argv)
    {
        try {
            Application* app = CreateApplication(argc, argv);
            CF_CORE_ASSERT(app, "Client Application is null!");
            app->Run();
            delete app;
            return 0;
        }
        catch (const std::exception& e) {
            CF_CORE_FATAL("Unhandled exception: {}", e.what());
            return 1;
        }
        catch (...) {
            CF_CORE_FATAL("Unknown unhandled exception — application terminated.");
            return 1;
        }
    }

    int Main(int argc, char** argv)
    {
#if defined(CF_PLATFORM_WINDOWS)
        // Outer SEH layer catches structural exceptions (access violations, etc.)
        // that C++ try/catch cannot intercept.  GetExceptionCode() is a compiler
        // intrinsic; the numeric value identifies the fault in a debugger/crash dump.
        __try {
            return RunApplication(argc, argv);
        }
        __except (EXCEPTION_EXECUTE_HANDLER) {
            unsigned const code = GetExceptionCode();
            CF_CORE_FATAL("Structured exception 0x{:08X} — application terminated.", code);
            return static_cast<int>(code);
        }
#else
        return RunApplication(argc, argv);
#endif
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
