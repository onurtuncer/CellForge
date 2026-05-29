// ------------------------------------------------------------------------------------
// Project: CellForge
// Copyright (C) 2026, Melina Aero Teknoloji Gelistirme ve Dizayn Burosu A.S., Istanbul
// Author: Onur Tuncer, PhD
//
// SPDX-License-Identifier: LGPL-2.1-only
// License-Filename: LICENSE
// ------------------------------------------------------------------------------------

#pragma once

#include "CellForge/ECS/Entity.h"

#include <flecs.h>

#include <string_view>

namespace CellForge {

// ─── World ────────────────────────────────────────────────────────────────────
// Owns the flecs world and provides the primary ECS surface:
//   • Entity creation and name-based lookup
//   • Component pre-registration (stable names for the flecs explorer)
//   • Single-component iteration via each<T>
//   • System registration — returns the flecs system builder so callers can
//     chain .kind() / .each() without this wrapper duplicating the builder API
//   • progress() to advance all systems one tick
//
// Use native() for advanced flecs features: multi-component queries, observers,
// relationship pairs, pipeline customisation, etc.

class World {
public:
    World() = default;

    // Non-copyable — owns the flecs world.
    World(const World&)            = delete;
    World& operator=(const World&) = delete;

    // ── Entity creation ───────────────────────────────────────────────────────

    Entity entity()                  { return Entity(m_world.entity()); }
    Entity entity(std::string_view name) { return Entity(m_world.entity(name.data())); }

    // O(log n) lookup by name. Returns an invalid Entity if not found.
    Entity lookup(std::string_view name) const {
        return Entity(m_world.lookup(name.data()));
    }

    // ── Component registration ────────────────────────────────────────────────
    // Pre-registering assigns a stable string name visible in the flecs explorer
    // and ensures consistent component IDs across shared libraries.

    template<typename T>
    void component(std::string_view name = "") {
        if (name.empty())
            m_world.component<T>();
        else
            m_world.component<T>(name.data());
    }

    // ── Iteration ─────────────────────────────────────────────────────────────
    // Fn signature: void(Entity, T&)      — for each<T>
    //               void(Entity, const T&) — for each<const T>
    //
    // Example:
    //   world.each<Name>([](Entity e, Name& n)       { n.value = "X"; });
    //   world.each<const Name>([](Entity e, const Name& n) { use(n.value); });

    template<typename T, typename Fn>
    void each(Fn&& fn) {
        m_world.each<T>([&fn](flecs::entity e, T& t) {
            fn(Entity(e), t);
        });
    }

    // ── Systems ───────────────────────────────────────────────────────────────
    // Returns the flecs system builder — chain .kind() and .each() on it.
    //
    // Example:
    //   world.system<const LocalTransform, WorldTransform>("TransformSystem")
    //       .kind(flecs::OnUpdate)
    //       .each([](flecs::entity e, const LocalTransform& l, WorldTransform& w) {
    //           ...
    //       });

    template<typename... Components>
    auto system(std::string_view name) {
        return m_world.system<Components...>(name.data());
    }

    // ── Simulation ────────────────────────────────────────────────────────────

    void progress(float deltaSeconds = 0.0f) { m_world.progress(deltaSeconds); }

    // ── Escape hatch ──────────────────────────────────────────────────────────

    flecs::world&       native()       { return m_world; }
    const flecs::world& native() const { return m_world; }

private:
    flecs::world m_world;
};

} // namespace CellForge
