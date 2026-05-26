// ------------------------------------------------------------------------------------
// Project: CellForge
// Copyright (C) 2026, Melina Aero Teknoloji Gelistirme ve Dizayn Burosu A.S., Istanbul
// Author: Onur Tuncer, PhD
//
// SPDX-License-Identifier: LGPL-2.1-only
// License-Filename: LICENSE
// ------------------------------------------------------------------------------------

#pragma once

#include "IApplicationPlatform.h"

#include <QMainWindow>
#include <QWidget>

#include <memory>
#include <string>

namespace CellForge
{
    class QtWindow final : public IWindow
    {
    public:
        QtWindow(const std::string& title, int width, int height)
        {
            m_Window = std::make_unique<QMainWindow>();

            m_Window->SetWindowTitle(QString::fromStdString(title));
            m_Window->Resize(width, height);
        }

        ~QtWindow() override = default;

        void Show() override
        {
            m_window->show();
        }

        void Close() override
        {
            m_window->close();
        }

        void SetTitle(const std::string& title) override
        {
            m_window->setWindowTitle(QString::fromStdString(title));
        }

        void SetSize(int width, int height) override
        {
            m_window->resize(width, height);
        }

        void SetCentralViewport(
            std::shared_ptr<IViewportWidget> viewport) override
        {
            m_viewport = std::move(viewport);

            auto* widget =
                static_cast<QWidget*>(m_viewport->NativeHandle());

            m_window->setCentralWidget(widget);
        }

        void* NativeHandle() override
        {
            return m_window.get();
        }

        QMainWindow* QtMainWindow()
        {
            return m_window.get();
        }

    private:
        std::unique_ptr<QMainWindow> m_Window;
        std::shared_ptr<IViewportWidget> m_Viewport;
    };
}