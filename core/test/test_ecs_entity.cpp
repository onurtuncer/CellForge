// ------------------------------------------------------------------------------------
// Project: CellForge
// Copyright (C) 2026, Melina Aero Teknoloji Gelistirme ve Dizayn Burosu A.S., Istanbul
// Author: Onur Tuncer, PhD
//
// SPDX-License-Identifier: LGPL-2.1-only
// License-Filename: LICENSE
// ------------------------------------------------------------------------------------

#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>

#include <CellForge/ECS/World.h>

#include <algorithm>

using namespace CellForge;

// ─── Test components ──────────────────────────────────────────────────────────

struct Position { float x = 0.f, y = 0.f, z = 0.f; };
struct Velocity { float x = 0.f, y = 0.f, z = 0.f; };
struct MarkerTag {};

// ─── Identity ─────────────────────────────────────────────────────────────────

TEST_CASE("Entity: default-constructed entity is invalid", "[ecs][entity]") {
    Entity e;
    CHECK_FALSE(e.valid());
    CHECK_FALSE(static_cast<bool>(e));
}

TEST_CASE("Entity: entity created via World is valid", "[ecs][entity]") {
    World world;
    Entity e = world.entity();
    CHECK(e.valid());
    CHECK(static_cast<bool>(e));
}

TEST_CASE("Entity: id is non-zero for a live entity", "[ecs][entity]") {
    World world;
    Entity e = world.entity();
    CHECK(e.id() != 0u);
}

TEST_CASE("Entity: two entities have different ids", "[ecs][entity]") {
    World world;
    Entity a = world.entity();
    Entity b = world.entity();
    CHECK(a.id() != b.id());
}

// ─── Name ─────────────────────────────────────────────────────────────────────

TEST_CASE("Entity: name is empty for an anonymous entity", "[ecs][entity]") {
    World world;
    Entity e = world.entity();
    CHECK(e.name().empty());
}

TEST_CASE("Entity: entity(name) stores the name", "[ecs][entity]") {
    World world;
    Entity e = world.entity("Robot");
    CHECK(e.name() == "Robot");
}

TEST_CASE("Entity: setName updates the name", "[ecs][entity]") {
    World world;
    Entity e = world.entity();
    e.setName("Link0");
    CHECK(e.name() == "Link0");
}

TEST_CASE("Entity: setName returns *this for chaining", "[ecs][entity]") {
    World world;
    Entity e = world.entity();
    Entity& ref = e.setName("X");
    CHECK(&ref == &e);
}

// ─── Tags ─────────────────────────────────────────────────────────────────────

TEST_CASE("Entity: has returns false before add", "[ecs][entity][tag]") {
    World world;
    Entity e = world.entity();
    CHECK_FALSE(e.has<MarkerTag>());
}

TEST_CASE("Entity: has returns true after add", "[ecs][entity][tag]") {
    World world;
    Entity e = world.entity();
    e.add<MarkerTag>();
    CHECK(e.has<MarkerTag>());
}

TEST_CASE("Entity: has returns false after remove", "[ecs][entity][tag]") {
    World world;
    Entity e = world.entity();
    e.add<MarkerTag>();
    e.remove<MarkerTag>();
    CHECK_FALSE(e.has<MarkerTag>());
}

TEST_CASE("Entity: add returns *this for chaining", "[ecs][entity][tag]") {
    World world;
    Entity e = world.entity();
    Entity& ref = e.add<MarkerTag>();
    CHECK(&ref == &e);
}

TEST_CASE("Entity: remove returns *this for chaining", "[ecs][entity][tag]") {
    World world;
    Entity e = world.entity();
    e.add<MarkerTag>();
    Entity& ref = e.remove<MarkerTag>();
    CHECK(&ref == &e);
}

// ─── Data components ──────────────────────────────────────────────────────────

TEST_CASE("Entity: get returns nullptr when component is absent", "[ecs][entity][component]") {
    World world;
    Entity e = world.entity();
    CHECK(e.get<Position>() == nullptr);
}

TEST_CASE("Entity: get returns pointer after set", "[ecs][entity][component]") {
    World world;
    Entity e = world.entity();
    e.set(Position{1.f, 2.f, 3.f});
    const Position* p = e.get<Position>();
    REQUIRE(p != nullptr);
    CHECK(p->x == Catch::Approx(1.f));
    CHECK(p->y == Catch::Approx(2.f));
    CHECK(p->z == Catch::Approx(3.f));
}

TEST_CASE("Entity: getMut returns nullptr when component is absent", "[ecs][entity][component]") {
    World world;
    Entity e = world.entity();
    CHECK(e.getMut<Position>() == nullptr);
}

TEST_CASE("Entity: getMut allows in-place mutation", "[ecs][entity][component]") {
    World world;
    Entity e = world.entity();
    e.set(Position{0.f, 0.f, 0.f});
    e.getMut<Position>()->x = 42.f;
    CHECK(e.get<Position>()->x == Catch::Approx(42.f));
}

TEST_CASE("Entity: setting a component twice keeps the latest value", "[ecs][entity][component]") {
    World world;
    Entity e = world.entity();
    e.set(Position{1.f, 0.f, 0.f});
    e.set(Position{9.f, 0.f, 0.f});
    CHECK(e.get<Position>()->x == Catch::Approx(9.f));
}

TEST_CASE("Entity: multiple component types are independent", "[ecs][entity][component]") {
    World world;
    Entity e = world.entity();
    e.set(Position{1.f, 2.f, 3.f});
    e.set(Velocity{4.f, 5.f, 6.f});
    REQUIRE(e.get<Position>() != nullptr);
    REQUIRE(e.get<Velocity>() != nullptr);
    CHECK(e.get<Position>()->x == Catch::Approx(1.f));
    CHECK(e.get<Velocity>()->x == Catch::Approx(4.f));
}

TEST_CASE("Entity: set returns *this for chaining", "[ecs][entity][component]") {
    World world;
    Entity e = world.entity();
    Entity& ref = e.set(Position{});
    CHECK(&ref == &e);
}

// ─── Method chaining ──────────────────────────────────────────────────────────

TEST_CASE("Entity: add/set/setName can be chained", "[ecs][entity]") {
    World world;
    Entity e = world.entity();
    e.add<MarkerTag>().set(Position{1.f, 2.f, 3.f}).setName("TestNode");
    CHECK(e.has<MarkerTag>());
    CHECK(e.get<Position>()->x == Catch::Approx(1.f));
    CHECK(e.name() == "TestNode");
}

// ─── Hierarchy ────────────────────────────────────────────────────────────────

TEST_CASE("Entity: childOf sets parent correctly", "[ecs][entity][hierarchy]") {
    World world;
    Entity parent = world.entity("parent");
    Entity child  = world.entity("child");
    child.childOf(parent);
    CHECK(child.parent() == parent);
}

TEST_CASE("Entity: parent of a root entity is invalid", "[ecs][entity][hierarchy]") {
    World world;
    Entity root = world.entity("root");
    CHECK_FALSE(root.parent().valid());
}

TEST_CASE("Entity: children returns all direct children", "[ecs][entity][hierarchy]") {
    World world;
    Entity parent = world.entity("parent");
    Entity c1     = world.entity("c1");
    Entity c2     = world.entity("c2");
    Entity c3     = world.entity("c3");
    c1.childOf(parent);
    c2.childOf(parent);
    c3.childOf(parent);

    auto kids = parent.children();
    REQUIRE(kids.size() == 3);
    CHECK(std::find(kids.begin(), kids.end(), c1) != kids.end());
    CHECK(std::find(kids.begin(), kids.end(), c2) != kids.end());
    CHECK(std::find(kids.begin(), kids.end(), c3) != kids.end());
}

TEST_CASE("Entity: children is empty for a leaf entity", "[ecs][entity][hierarchy]") {
    World world;
    Entity leaf = world.entity("leaf");
    CHECK(leaf.children().empty());
}

TEST_CASE("Entity: childOf returns *this for chaining", "[ecs][entity][hierarchy]") {
    World world;
    Entity parent = world.entity("parent");
    Entity child  = world.entity("child");
    Entity& ref = child.childOf(parent);
    CHECK(&ref == &child);
}

// ─── Lifecycle ────────────────────────────────────────────────────────────────

TEST_CASE("Entity: destruct makes the entity invalid", "[ecs][entity][lifecycle]") {
    World world;
    Entity e = world.entity("temp");
    REQUIRE(e.valid());
    e.destruct();
    CHECK_FALSE(e.valid());
}

// ─── Equality ─────────────────────────────────────────────────────────────────

TEST_CASE("Entity: copies are equal", "[ecs][entity]") {
    World world;
    Entity a = world.entity("a");
    Entity b = a;
    CHECK(a == b);
    CHECK_FALSE(a != b);
}

TEST_CASE("Entity: different entities are not equal", "[ecs][entity]") {
    World world;
    Entity a = world.entity("a");
    Entity b = world.entity("b");
    CHECK(a != b);
    CHECK_FALSE(a == b);
}

// ─── native() ─────────────────────────────────────────────────────────────────

TEST_CASE("Entity: native returns the underlying flecs entity", "[ecs][entity]") {
    World world;
    Entity e = world.entity("native_test");
    flecs::entity fe = e.native();
    CHECK(fe.id() == e.id());
    CHECK(fe.is_alive());
}
