// ------------------------------------------------------------------------------------
// Project: CellForge
// Copyright (C) 2026, Melina Aero Teknoloji Gelistirme ve Dizayn Burosu A.S., Istanbul
// Author: Onur Tuncer, PhD
//
// SPDX-License-Identifier: LGPL-2.1-only
// License-Filename: LICENSE
// ------------------------------------------------------------------------------------

#include <catch2/catch_test_macros.hpp>

#include <CellForge/ecs/UUID.h>

#include <unordered_set>

using namespace CellForge;

// ─── UUID ─────────────────────────────────────────────────────────────────────

TEST_CASE("UUID default construction produces a non-zero value", "[uuid]")
{
    UUID id;
    CHECK(static_cast<uint64_t>(id) != 0u);
}

TEST_CASE("UUID two successive default constructions differ", "[uuid]")
{
    // 64-bit random space makes collision astronomically unlikely.
    UUID a, b;
    CHECK(static_cast<uint64_t>(a) != static_cast<uint64_t>(b));
}

TEST_CASE("UUID value constructor preserves the exact value", "[uuid]")
{
    UUID id(0xDEADBEEFCAFEBABEull);
    CHECK(static_cast<uint64_t>(id) == 0xDEADBEEFCAFEBABEull);
}

TEST_CASE("UUID value constructor accepts zero", "[uuid]")
{
    UUID id(0u);
    CHECK(static_cast<uint64_t>(id) == 0u);
}

TEST_CASE("UUID copy constructor preserves the value", "[uuid]")
{
    UUID original(12345u);
    UUID copy(original);
    CHECK(static_cast<uint64_t>(copy) == 12345u);
}

TEST_CASE("UUID mutable cast operator returns the stored value", "[uuid]")
{
    UUID id(99u);
    CHECK(static_cast<uint64_t>(id) == 99u);
}

TEST_CASE("UUID const cast operator returns the stored value", "[uuid]")
{
    const UUID id(77u);
    CHECK(static_cast<uint64_t>(id) == 77u);
}

TEST_CASE("UUID is usable as an unordered_set key", "[uuid]")
{
    std::unordered_set<UUID, std::hash<UUID>> ids;

    UUID a(1u), b(2u), dup_a(1u);
    ids.insert(a);
    ids.insert(b);
    ids.insert(dup_a); // same value as a — must not grow the set

    CHECK(ids.size() == 2u);
    CHECK(ids.count(UUID(1u)) == 1u);
    CHECK(ids.count(UUID(2u)) == 1u);
    CHECK(ids.count(UUID(3u)) == 0u);
}

TEST_CASE("UUID hash is consistent across repeated calls", "[uuid]")
{
    UUID id(42u);
    std::hash<UUID> hasher;
    CHECK(hasher(id) == hasher(id));
}

TEST_CASE("UUID hash differs for different values", "[uuid]")
{
    std::hash<UUID> hasher;
    // Not a guarantee (collisions exist), but true for these simple values.
    CHECK(hasher(UUID(1u)) != hasher(UUID(2u)));
}

// ─── UUID32 ───────────────────────────────────────────────────────────────────

TEST_CASE("UUID32 default construction produces a non-zero value", "[uuid32]")
{
    UUID32 id;
    CHECK(static_cast<uint32_t>(id) != 0u);
}

TEST_CASE("UUID32 two successive default constructions differ", "[uuid32]")
{
    UUID32 a, b;
    CHECK(static_cast<uint32_t>(a) != static_cast<uint32_t>(b));
}

TEST_CASE("UUID32 value constructor preserves the exact value", "[uuid32]")
{
    UUID32 id(0xCAFEBABEu);
    CHECK(static_cast<uint32_t>(id) == 0xCAFEBABEu);
}

TEST_CASE("UUID32 value constructor accepts zero", "[uuid32]")
{
    UUID32 id(0u);
    CHECK(static_cast<uint32_t>(id) == 0u);
}

TEST_CASE("UUID32 copy constructor preserves the value", "[uuid32]")
{
    UUID32 original(7u);
    UUID32 copy(original);
    CHECK(static_cast<uint32_t>(copy) == 7u);
}

TEST_CASE("UUID32 mutable cast operator returns the stored value", "[uuid32]")
{
    UUID32 id(55u);
    CHECK(static_cast<uint32_t>(id) == 55u);
}

TEST_CASE("UUID32 const cast operator returns the stored value", "[uuid32]")
{
    const UUID32 id(33u);
    CHECK(static_cast<uint32_t>(id) == 33u);
}

TEST_CASE("UUID32 is usable as an unordered_set key", "[uuid32]")
{
    std::unordered_set<UUID32, std::hash<UUID32>> ids;

    UUID32 a(10u), b(20u), dup_a(10u);
    ids.insert(a);
    ids.insert(b);
    ids.insert(dup_a);

    CHECK(ids.size() == 2u);
    CHECK(ids.count(UUID32(10u)) == 1u);
    CHECK(ids.count(UUID32(99u)) == 0u);
}

TEST_CASE("UUID32 hash is consistent across repeated calls", "[uuid32]")
{
    UUID32 id(42u);
    std::hash<UUID32> hasher;
    CHECK(hasher(id) == hasher(id));
}
