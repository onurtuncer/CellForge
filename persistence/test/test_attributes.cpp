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

#include <CellForge/OcafPersistenceBackend.h>

#include <cstdint>
#include <limits>

using namespace CellForge;

// ─── Fixture ──────────────────────────────────────────────────────────────────

struct AttrFixture {
    OcafPersistenceBackend backend;
    DocumentId doc;
    NodeId node;

    AttrFixture() {
        doc = backend.newDocument("AttrTest");
        backend.beginTransaction(doc, "setup");
        node = backend.newChild(backend.rootNode(doc));
        backend.commitTransaction(doc);
    }

    template<typename Fn>
    void txn(Fn fn) {
        backend.beginTransaction(doc, "");
        fn();
        backend.commitTransaction(doc);
    }
};

// ─── Real (double) ────────────────────────────────────────────────────────────

TEST_CASE("Attribute: real (double)", "[persistence][attributes]") {
    AttrFixture f;

    SECTION("roundtrip") {
        f.txn([&] { f.backend.setReal(f.node, "mass", 9.81); });
        auto v = f.backend.getReal(f.node, "mass");
        REQUIRE(v.has_value());
        REQUIRE(*v == Catch::Approx(9.81));
    }

    SECTION("absent key returns nullopt") {
        REQUIRE_FALSE(f.backend.getReal(f.node, "missing").has_value());
    }

    SECTION("zero") {
        f.txn([&] { f.backend.setReal(f.node, "z", 0.0); });
        REQUIRE(*f.backend.getReal(f.node, "z") == Catch::Approx(0.0));
    }

    SECTION("negative value") {
        f.txn([&] { f.backend.setReal(f.node, "delta", -1.23e10); });
        REQUIRE(*f.backend.getReal(f.node, "delta") == Catch::Approx(-1.23e10));
    }

    SECTION("very small value") {
        f.txn([&] { f.backend.setReal(f.node, "tiny", 1e-300); });
        REQUIRE(*f.backend.getReal(f.node, "tiny") == Catch::Approx(1e-300));
    }

    SECTION("overwrite updates the stored value") {
        f.txn([&] { f.backend.setReal(f.node, "x", 1.0); });
        f.txn([&] { f.backend.setReal(f.node, "x", 2.0); });
        REQUIRE(*f.backend.getReal(f.node, "x") == Catch::Approx(2.0));
    }

    SECTION("multiple keys on the same node are independent") {
        f.txn([&] {
            f.backend.setReal(f.node, "a", 1.0);
            f.backend.setReal(f.node, "b", 2.0);
        });
        REQUIRE(*f.backend.getReal(f.node, "a") == Catch::Approx(1.0));
        REQUIRE(*f.backend.getReal(f.node, "b") == Catch::Approx(2.0));
    }
}

// ─── Integer (int64_t) ────────────────────────────────────────────────────────

TEST_CASE("Attribute: integer (int64_t)", "[persistence][attributes]") {
    AttrFixture f;

    SECTION("roundtrip") {
        f.txn([&] { f.backend.setInt(f.node, "count", 42LL); });
        auto v = f.backend.getInt(f.node, "count");
        REQUIRE(v.has_value());
        REQUIRE(*v == 42LL);
    }

    SECTION("absent key returns nullopt") {
        REQUIRE_FALSE(f.backend.getInt(f.node, "missing").has_value());
    }

    SECTION("zero") {
        f.txn([&] { f.backend.setInt(f.node, "z", 0LL); });
        REQUIRE(*f.backend.getInt(f.node, "z") == 0LL);
    }

    SECTION("negative value") {
        f.txn([&] { f.backend.setInt(f.node, "neg", -99999LL); });
        REQUIRE(*f.backend.getInt(f.node, "neg") == -99999LL);
    }

    SECTION("int64 max") {
        constexpr int64_t big = std::numeric_limits<int64_t>::max();
        f.txn([&] { f.backend.setInt(f.node, "max", big); });
        REQUIRE(*f.backend.getInt(f.node, "max") == big);
    }

    SECTION("int64 min") {
        constexpr int64_t small = std::numeric_limits<int64_t>::min();
        f.txn([&] { f.backend.setInt(f.node, "min", small); });
        REQUIRE(*f.backend.getInt(f.node, "min") == small);
    }

    SECTION("overwrite") {
        f.txn([&] { f.backend.setInt(f.node, "n", 1LL); });
        f.txn([&] { f.backend.setInt(f.node, "n", 2LL); });
        REQUIRE(*f.backend.getInt(f.node, "n") == 2LL);
    }
}

// ─── Boolean ──────────────────────────────────────────────────────────────────

TEST_CASE("Attribute: boolean", "[persistence][attributes]") {
    AttrFixture f;

    SECTION("true roundtrip") {
        f.txn([&] { f.backend.setBool(f.node, "flag", true); });
        auto v = f.backend.getBool(f.node, "flag");
        REQUIRE(v.has_value());
        REQUIRE(*v == true);
    }

    SECTION("false roundtrip") {
        f.txn([&] { f.backend.setBool(f.node, "flag", false); });
        REQUIRE(*f.backend.getBool(f.node, "flag") == false);
    }

    SECTION("absent key returns nullopt") {
        REQUIRE_FALSE(f.backend.getBool(f.node, "nope").has_value());
    }

    SECTION("toggle true then false") {
        f.txn([&] { f.backend.setBool(f.node, "t", true); });
        REQUIRE(*f.backend.getBool(f.node, "t") == true);
        f.txn([&] { f.backend.setBool(f.node, "t", false); });
        REQUIRE(*f.backend.getBool(f.node, "t") == false);
    }
}

// ─── String ───────────────────────────────────────────────────────────────────

TEST_CASE("Attribute: string", "[persistence][attributes]") {
    AttrFixture f;

    SECTION("roundtrip") {
        f.txn([&] { f.backend.setString(f.node, "label", "hello world"); });
        auto v = f.backend.getString(f.node, "label");
        REQUIRE(v.has_value());
        REQUIRE(*v == "hello world");
    }

    SECTION("absent key returns nullopt") {
        REQUIRE_FALSE(f.backend.getString(f.node, "missing").has_value());
    }

    SECTION("empty string") {
        f.txn([&] { f.backend.setString(f.node, "empty", ""); });
        REQUIRE(*f.backend.getString(f.node, "empty") == "");
    }

    SECTION("string with special characters") {
        const std::string val = "line1\nline2\ttab";
        f.txn([&] { f.backend.setString(f.node, "s", val); });
        REQUIRE(*f.backend.getString(f.node, "s") == val);
    }

    SECTION("overwrite") {
        f.txn([&] { f.backend.setString(f.node, "s", "first"); });
        f.txn([&] { f.backend.setString(f.node, "s", "second"); });
        REQUIRE(*f.backend.getString(f.node, "s") == "second");
    }

    SECTION("long string") {
        const std::string val(4096, 'x');
        f.txn([&] { f.backend.setString(f.node, "long", val); });
        REQUIRE(*f.backend.getString(f.node, "long") == val);
    }
}

// ─── removeAttribute ─────────────────────────────────────────────────────────

TEST_CASE("removeAttribute", "[persistence][attributes]") {
    AttrFixture f;

    SECTION("returns true and clears a real attribute") {
        f.txn([&] { f.backend.setReal(f.node, "tmp", 1.0); });
        bool removed = false;
        f.txn([&] { removed = f.backend.removeAttribute(f.node, "tmp"); });
        REQUIRE(removed);
        REQUIRE_FALSE(f.backend.getReal(f.node, "tmp").has_value());
    }

    SECTION("returns true and clears an int attribute") {
        f.txn([&] { f.backend.setInt(f.node, "tmp", 1LL); });
        bool removed = false;
        f.txn([&] { removed = f.backend.removeAttribute(f.node, "tmp"); });
        REQUIRE(removed);
        REQUIRE_FALSE(f.backend.getInt(f.node, "tmp").has_value());
    }

    SECTION("returns false for a key that was never set") {
        bool removed = false;
        f.txn([&] { removed = f.backend.removeAttribute(f.node, "ghost"); });
        REQUIRE_FALSE(removed);
    }

    SECTION("removing one key does not affect sibling keys") {
        f.txn([&] {
            f.backend.setReal(f.node, "keep", 7.0);
            f.backend.setReal(f.node, "drop", 8.0);
        });
        f.txn([&] { f.backend.removeAttribute(f.node, "drop"); });
        REQUIRE(*f.backend.getReal(f.node, "keep") == Catch::Approx(7.0));
        REQUIRE_FALSE(f.backend.getReal(f.node, "drop").has_value());
    }
}

// ─── Node name ────────────────────────────────────────────────────────────────

TEST_CASE("Node name", "[persistence][attributes]") {
    AttrFixture f;

    SECTION("setName / getName roundtrip") {
        f.txn([&] { f.backend.setName(f.node, "MyPart"); });
        REQUIRE(f.backend.getName(f.node) == "MyPart");
    }

    SECTION("empty name is valid") {
        f.txn([&] { f.backend.setName(f.node, ""); });
        REQUIRE(f.backend.getName(f.node) == "");
    }

    SECTION("rename overwrites previous name") {
        f.txn([&] { f.backend.setName(f.node, "First"); });
        f.txn([&] { f.backend.setName(f.node, "Second"); });
        REQUIRE(f.backend.getName(f.node) == "Second");
    }

    SECTION("siblings can share the same name") {
        NodeId root = f.backend.rootNode(f.doc);
        NodeId n2 = NullNode;
        f.txn([&] { n2 = f.backend.newChild(root); });
        f.txn([&] {
            f.backend.setName(f.node, "Duplicate");
            f.backend.setName(n2,     "Duplicate");
        });
        REQUIRE(f.backend.getName(f.node) == "Duplicate");
        REQUIRE(f.backend.getName(n2)     == "Duplicate");
    }
}

// ─── Shape reference ──────────────────────────────────────────────────────────

TEST_CASE("Shape reference", "[persistence][attributes]") {
    AttrFixture f;

    SECTION("getShape returns NullShape when not set") {
        REQUIRE(f.backend.getShape(f.node) == NullShape);
    }

    SECTION("setShape / getShape roundtrip") {
        constexpr ShapeId sid = 0xDEADBEEFDEADBEEFULL;
        f.txn([&] { f.backend.setShape(f.node, sid); });
        REQUIRE(f.backend.getShape(f.node) == sid);
    }

    SECTION("overwrite shape id") {
        f.txn([&] { f.backend.setShape(f.node, 1ULL); });
        f.txn([&] { f.backend.setShape(f.node, 2ULL); });
        REQUIRE(f.backend.getShape(f.node) == 2ULL);
    }

    SECTION("shape id 1 (smallest non-null)") {
        f.txn([&] { f.backend.setShape(f.node, 1ULL); });
        REQUIRE(f.backend.getShape(f.node) == 1ULL);
    }

    SECTION("max uint64 shape id") {
        constexpr ShapeId maxId = std::numeric_limits<uint64_t>::max();
        f.txn([&] { f.backend.setShape(f.node, maxId); });
        REQUIRE(f.backend.getShape(f.node) == maxId);
    }

    SECTION("shape ids are per-node") {
        NodeId n2 = NullNode;
        f.txn([&] { n2 = f.backend.newChild(f.backend.rootNode(f.doc)); });
        f.txn([&] {
            f.backend.setShape(f.node, 10ULL);
            f.backend.setShape(n2,     20ULL);
        });
        REQUIRE(f.backend.getShape(f.node) == 10ULL);
        REQUIRE(f.backend.getShape(n2)     == 20ULL);
    }
}

// ─── Assembly transform ───────────────────────────────────────────────────────

TEST_CASE("Assembly transform", "[persistence][attributes]") {
    AttrFixture f;

    SECTION("default transform is identity") {
        Transform t  = f.backend.getTransform(f.node);
        Transform id = Transform::identity();
        for (int i = 0; i < 16; ++i)
            REQUIRE(t.data[i] == Catch::Approx(id.data[i]));
    }

    SECTION("setTransform / getTransform roundtrip with translation") {
        Transform t;
        t(0, 3) = 1.0;
        t(1, 3) = 2.0;
        t(2, 3) = 3.0;

        f.txn([&] { f.backend.setTransform(f.node, t); });
        Transform r = f.backend.getTransform(f.node);
        for (int i = 0; i < 16; ++i)
            REQUIRE(r.data[i] == Catch::Approx(t.data[i]));
    }

    SECTION("setTransform / getTransform roundtrip with full matrix") {
        Transform t;
        for (int i = 0; i < 16; ++i)
            t.data[i] = static_cast<double>(i) * 0.1;

        f.txn([&] { f.backend.setTransform(f.node, t); });
        Transform r = f.backend.getTransform(f.node);
        for (int i = 0; i < 16; ++i)
            REQUIRE(r.data[i] == Catch::Approx(t.data[i]));
    }

    SECTION("overwrite replaces previous transform") {
        Transform t1, t2;
        t1(0, 3) = 5.0;
        t2(0, 3) = 10.0;

        f.txn([&] { f.backend.setTransform(f.node, t1); });
        f.txn([&] { f.backend.setTransform(f.node, t2); });
        REQUIRE(f.backend.getTransform(f.node)(0, 3) == Catch::Approx(10.0));
    }

    SECTION("transforms are per-node") {
        NodeId n2 = NullNode;
        f.txn([&] { n2 = f.backend.newChild(f.backend.rootNode(f.doc)); });

        Transform ta, tb;
        ta(0, 3) = 1.0;
        tb(0, 3) = 9.0;

        f.txn([&] {
            f.backend.setTransform(f.node, ta);
            f.backend.setTransform(n2,     tb);
        });

        REQUIRE(f.backend.getTransform(f.node)(0, 3) == Catch::Approx(1.0));
        REQUIRE(f.backend.getTransform(n2)(0, 3)     == Catch::Approx(9.0));
    }
}
