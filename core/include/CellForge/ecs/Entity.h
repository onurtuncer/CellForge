// ------------------------------------------------------------------------------------
// Project: CellForge
// Copyright (C) 2026, Melina Aero Teknoloji Gelistirme ve Dizayn Burosu A.S., Istanbul
// Author: Onur Tuncer, PhD
//
// SPDX-License-Identifier: LGPL-2.1-only
// License-Filename: LICENSE
// ------------------------------------------------------------------------------------

#pragma once

#include <flecs.h>

#include <string>
#include <string_view>
#include <vector>

namespace CellForge {

// ─── Entity ───────────────────────────────────────────────────────────────────
// Lightweight wrapper around flecs::entity.  Copyable and cheap to pass by
// value — the underlying handle is a 64-bit integer.
//
// Template methods cover the common case.  Use native() to reach advanced
// flecs features (observers, query builders, relationship pairs, etc.).

class Entity {
public:
    Entity() = default;
    explicit Entity(flecs::entity e) : m_entity(e) {}

    // ── Identity ──────────────────────────────────────────────────────────────

    uint64_t    id()    const { return m_entity.id(); }
    bool        valid() const { return m_entity.is_valid() && m_entity.is_alive(); }
    std::string name()  const { const char* n = m_entity.name(); return n ? n : ""; }

    Entity& setName(std::string_view name) {
        m_entity.set_name(name.data());
        return *this;
    }

    // ── Tags (zero-size markers) ──────────────────────────────────────────────

    template<typename T>
    Entity& add() { m_entity.add<T>(); return *this; }

    template<typename T>
    Entity& remove() { m_entity.remove<T>(); return *this; }

    template<typename T>
    bool has() const { return m_entity.has<T>(); }

    // ── Data components ───────────────────────────────────────────────────────

    template<typename T>
    Entity& set(T value) { m_entity.set<T>(std::move(value)); return *this; }

    // Returns nullptr if the component is absent.
    template<typename T>
    const T* get() const { return m_entity.get<T>(); }

    // Returns nullptr if the component is absent.
    template<typename T>
    T* getMut() { return m_entity.get_mut<T>(); }

    // ── Hierarchy ─────────────────────────────────────────────────────────────

    Entity& childOf(Entity parent) {
        m_entity.child_of(parent.m_entity);
        return *this;
    }

    Entity parent() const { return Entity(m_entity.parent()); }

    std::vector<Entity> children() const {
        std::vector<Entity> result;
        m_entity.children([&result](flecs::entity child) {
            result.emplace_back(child);
        });
        return result;
    }

    // ── Lifecycle ─────────────────────────────────────────────────────────────

    void destruct() { m_entity.destruct(); }

    // ── Escape hatch ──────────────────────────────────────────────────────────

    flecs::entity native() const { return m_entity; }

    // ── Comparisons ───────────────────────────────────────────────────────────

    bool operator==(const Entity& o) const { return m_entity == o.m_entity; }
    bool operator!=(const Entity& o) const { return m_entity != o.m_entity; }
    explicit operator bool() const { return valid(); }

private:
    flecs::entity m_entity;
};

} // namespace CellForge
