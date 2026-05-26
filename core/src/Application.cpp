// ------------------------------------------------------------------------------------
// Project: CellForge
// Copyright (C) 2026, Melina Aero Teknoloji Gelistirme ve Dizayn Burosu A.S., Istanbul
// Author: Onur Tuncer, PhD
//
// SPDX-License-Identifier: LGPL-2.1-only
// License-Filename: LICENSE
// ------------------------------------------------------------------------------------

#include "CellForge/Application.h"
#include "CellForge/Log.h"
#include "CellForge/Assert.h"

namespace CellForge {

    Application* Application::s_Instance = nullptr;

    Application::Application(const ApplicationSpecification& spec,
                             Scope<IApplicationPlatform> platform)
        : m_Specification(spec)
        , m_Platform(std::move(platform))
    {
        CF_CORE_ASSERT(!s_Instance, "Application already exists!");
        s_Instance = this;
        Log::Init();
        CF_CORE_INFO("Application '{}' starting.", spec.Name);
    }

    Application::~Application()
    {
        CF_CORE_INFO("Application '{}' shutting down.", m_Specification.Name);
        Log::Shutdown();
        s_Instance = nullptr;
    }

    void Application::Run()
    {
        if (m_Platform) m_Platform->init();
        OnInit();
        while (m_Running)
        {
            if (m_Platform) m_Platform->pollEvents();
            ProcessEvents();
            OnUpdate();
        }
        OnShutdown();
        if (m_Platform) m_Platform->shutdown();
    }

    void Application::Close()
    {
        m_Running = false;
    }

    void Application::AddEventCallback(EventCallbackFn cb)
    {
        m_EventCallbacks.push_back(std::move(cb));
    }

    void Application::ProcessEvents()
    {
        std::deque<std::function<void()>> local;
        {
            std::scoped_lock lock(m_EventQueueMutex);
            local.swap(m_EventQueue);
        }
        for (auto& fn : local)
            fn();
    }

    void Application::OnEvent(Event& event)
    {
        for (auto& cb : m_EventCallbacks)
        {
            cb(event);
            if (event.handled)
                break;
        }
    }

} // namespace CellForge
