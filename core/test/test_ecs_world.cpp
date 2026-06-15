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

#include <CellForge/ecs/World.h>

using namespace CellForge;

// ─── Test components ──────────────────────────────────────────────────────────

struct Pos   { float x = 0.f, y = 0.f; };
struct Speed { float value = 0.f; };
struct ActiveTag {};

// ─── Entity creation ──────────────────────────────────────────────────────────

TEST_CASE("World: entity() returns a valid entity", "[ecs][world]") {
    World world;
    Entity e = world.entity();
    CHECK(e.valid());
}

TEST_CASE("World: entity(name) returns a valid named entity", "[ecs][world]") {
    World world;
    Entity e = world.entity("Foo");
    CHECK(e.valid());
    CHECK(e.name() == "Foo");
}

TEST_CASE("World: two entity() calls return distinct entities", "[ecs][world]") {
    World world;
    Entity a = world.entity();
    Entity b = world.entity();
    CHECK(a != b);
}

// ─── lookup ───────────────────────────────────────────────────────────────────

TEST_CASE("World: lookup finds a named entity", "[ecs][world]") {
    World world;
    Entity e = world.entity("FindMe");
    Entity found = world.lookup("FindMe");
    CHECK(found.valid());
    CHECK(found == e);
}

TEST_CASE("World: lookup returns an invalid entity for an unknown name", "[ecs][world]") {
    World world;
    Entity found = world.lookup("DoesNotExist");
    CHECK_FALSE(found.valid());
}

TEST_CASE("World: lookup is consistent with the entity name", "[ecs][world]") {
    World world;
    world.entity("Alpha");
    world.entity("Beta");
    Entity gamma = world.entity("Gamma");
    CHECK(world.lookup("Gamma") == gamma);
    CHECK(world.lookup("Alpha") != gamma);
}

// ─── component registration ───────────────────────────────────────────────────

TEST_CASE("World: component<T>() registers without error", "[ecs][world]") {
    World world;
    REQUIRE_NOTHROW(world.component<Pos>("Pos"));
    REQUIRE_NOTHROW(world.component<Speed>("Speed"));
}

TEST_CASE("World: component<T>() without name registers without error", "[ecs][world]") {
    World world;
    REQUIRE_NOTHROW(world.component<ActiveTag>());
}

// ─── each<T> ─────────────────────────────────────────────────────────────────

TEST_CASE("World: each<T> visits only entities that have T", "[ecs][world]") {
    World world;
    Entity e1 = world.entity("e1"); e1.set(Pos{1.f, 0.f});
    Entity e2 = world.entity("e2"); e2.set(Pos{2.f, 0.f});
    Entity e3 = world.entity("e3"); // no Pos

    int count = 0;
    world.each<Pos>([&](Entity, Pos&) { ++count; });
    CHECK(count == 2);
}

TEST_CASE("World: each<T> visits zero entities when none have T", "[ecs][world]") {
    World world;
    world.entity("a");
    world.entity("b");

    int count = 0;
    world.each<Speed>([&](Entity, Speed&) { ++count; });
    CHECK(count == 0);
}

TEST_CASE("World: each<T> provides mutable access", "[ecs][world]") {
    World world;
    Entity e = world.entity("e"); e.set(Pos{0.f, 0.f});

    world.each<Pos>([](Entity, Pos& p) { p.x = 99.f; });
    CHECK(e.get<Pos>()->x == Catch::Approx(99.f));
}

TEST_CASE("World: each<const T> provides read-only access", "[ecs][world]") {
    World world;
    Entity e1 = world.entity("e1"); e1.set(Pos{3.f, 0.f});
    Entity e2 = world.entity("e2"); e2.set(Pos{7.f, 0.f});

    float sum = 0.f;
    world.each<const Pos>([&](Entity, const Pos& p) { sum += p.x; });
    CHECK(sum == Catch::Approx(10.f));
}

TEST_CASE("World: each<T> passes the correct entity handle", "[ecs][world]") {
    World world;
    Entity e = world.entity("target"); e.set(Pos{});

    Entity visited;
    world.each<Pos>([&](Entity found, Pos&) { visited = found; });
    CHECK(visited == e);
}

TEST_CASE("World: each<T> visits all three entities with T", "[ecs][world]") {
    World world;
    for (int i = 0; i < 3; ++i)
        world.entity().set(Speed{static_cast<float>(i)});

    float sum = 0.f;
    world.each<Speed>([&](Entity, Speed& s) { sum += s.value; });
    CHECK(sum == Catch::Approx(0.f + 1.f + 2.f));
}

// ─── system and progress ──────────────────────────────────────────────────────

TEST_CASE("World: system registers and progress runs it", "[ecs][world]") {
    World world;

    int ticks = 0;
    world.system<Pos>("MoveSystem")
        .kind(flecs::OnUpdate)
        .each([&ticks](flecs::entity, Pos& p) {
            p.x += 1.f;
            ++ticks;
        });

    Entity e = world.entity("mover"); e.set(Pos{0.f, 0.f});

    world.progress(0.016f);
    CHECK(ticks == 1);
    CHECK(e.get<Pos>()->x == Catch::Approx(1.f));

    world.progress(0.016f);
    CHECK(ticks == 2);
    CHECK(e.get<Pos>()->x == Catch::Approx(2.f));
}

TEST_CASE("World: progress with no systems or entities does not throw", "[ecs][world]") {
    World world;
    REQUIRE_NOTHROW(world.progress());
    REQUIRE_NOTHROW(world.progress(0.016f));
}

TEST_CASE("World: system only runs for entities that have all queried components", "[ecs][world]") {
    World world;

    int count = 0;
    world.system<Pos, Speed>("PosSpeedSystem")
        .kind(flecs::OnUpdate)
        .each([&count](flecs::entity, Pos&, Speed&) { ++count; });

    world.entity("withBoth").set(Pos{}).set(Speed{});
    world.entity("posOnly").set(Pos{});
    world.entity("speedOnly").set(Speed{});

    world.progress();
    CHECK(count == 1);
}

// ─── native() ─────────────────────────────────────────────────────────────────

TEST_CASE("World: native returns a reference to the flecs world", "[ecs][world]") {
    World world;
    flecs::world& fw = world.native();

    // Verify we got the real world by creating an entity through both interfaces
    // and confirming they share the same entity space.
    Entity   via_wrapper = world.entity("wrapper_entity");
    auto     via_native  = fw.entity("native_entity");

    CHECK(fw.lookup("wrapper_entity").id() == via_wrapper.id());
    CHECK(world.lookup("native_entity") == Entity(via_native));
}

TEST_CASE("World: const native returns a const reference", "[ecs][world]") {
    World world;
    world.entity("X");
    const World& cw = world;
    const flecs::world& cfw = cw.native();
    CHECK(cfw.lookup("X").is_alive());
}
