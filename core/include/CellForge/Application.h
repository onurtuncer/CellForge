// ------------------------------------------------------------------------------------
// Project: CellForge
// Copyright (C) 2026, Melina Aero Teknoloji Gelistirme ve Dizayn Burosu A.S., Istanbul
// Author: Onur Tuncer, PhD
//
// SPDX-License-Identifier: LGPL-2.1-only
// License-Filename: LICENSE
// ------------------------------------------------------------------------------------

#pragma once

#include "CellForge/Base.h"
#include "CellForge/Event/Event.h"
#include "CellForge/IApplicationPlatform.h"

#include <functional>
#include <memory>
#include <string>
#include <vector>
#include <mutex>
#include <deque>

namespace CellForge {

    struct ApplicationSpecification {
        std::string Name = "CellForge App";
    };

    class CF_API Application {
        using EventCallbackFn = std::function<void(Event&)>;
    public:
        explicit Application(const ApplicationSpecification& spec = {},
                             Scope<IApplicationPlatform> platform = nullptr);
        virtual ~Application();

        void Run();
        void Close();

        virtual void OnInit() {}
        virtual void OnShutdown() {}
        virtual void OnUpdate() {}
        virtual void OnEvent(Event& event);

        void AddEventCallback(EventCallbackFn cb);

        template<typename TEvent, typename... TArgs>
        void DispatchEvent(TArgs&&... args)
        {
            auto event = std::make_shared<TEvent>(std::forward<TArgs>(args)...);
            std::scoped_lock lock(m_EventQueueMutex);
            m_EventQueue.emplace_back([event, this]() { OnEvent(*event); });
        }

        const ApplicationSpecification& GetSpecification() const { return m_Specification; }

        static Application& Get() { return *s_Instance; }

    private:
        void ProcessEvents();

    private:
        ApplicationSpecification       m_Specification;
        Scope<IApplicationPlatform>    m_Platform;
        bool                           m_Running = true;

        std::mutex                            m_EventQueueMutex;
        std::deque<std::function<void()>>     m_EventQueue;
        std::vector<EventCallbackFn>          m_EventCallbacks;

        static Application* s_Instance;
    };

    // Implemented by CLIENT
    Application* CreateApplication(int argc, char** argv);

} // namespace CellForge
