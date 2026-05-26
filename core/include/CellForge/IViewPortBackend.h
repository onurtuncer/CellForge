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

    class Entity;

    class IViewportBackend
{
public:
    virtual ~IViewportBackend() = default;

    virtual void Display(Entity entity) = 0;
    virtual void Hide(Entity entity) = 0;
    virtual void UpdateTransform(Entity entity) = 0;
    virtual std::optional<Entity> Pick(int x, int y) = 0;
    virtual void FitAll() = 0;
    virtual void Redraw() = 0;
};



}

