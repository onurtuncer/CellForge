// ------------------------------------------------------------------------------------
// Project: CellForge
// Copyright (C) 2026, Melina Aero Teknoloji Gelistirme ve Dizayn Burosu A.S., Istanbul
// Author: Onur Tuncer, PhD
//
// SPDX-License-Identifier: LGPL-2.1-only
// License-Filename: LICENSE
// ------------------------------------------------------------------------------------

#include <catch2/catch_test_macros.hpp>

#include <CellForge/OcafPersistenceBackend.h>

using namespace CellForge;

// ─── Fixture ──────────────────────────────────────────────────────────────────

struct NodeFixture {
    OcafPersistenceBackend backend;
    DocumentId doc;
    NodeId root;

    NodeFixture() {
        doc  = backend.newDocument("NodeTest");
        root = backend.rootNode(doc);
    }

    // Convenience: wrap a mutation in a transaction.
    template<typename Fn>
    void txn(Fn fn) {
        backend.beginTransaction(doc, "");
        fn();
        backend.commitTransaction(doc);
    }

    NodeId addChild(NodeId parent, std::string_view name = "") {
        NodeId n = NullNode;
        txn([&] {
            n = backend.newChild(parent);
            if (!name.empty()) backend.setName(n, name);
        });
        return n;
    }
};

// ─── Tests ────────────────────────────────────────────────────────────────────

TEST_CASE("rootNode", "[persistence][nodes]") {
    OcafPersistenceBackend backend;
    DocumentId doc = backend.newDocument("Root");

    SECTION("rootNode is not null") {
        REQUIRE(backend.rootNode(doc) != NullNode);
    }

    SECTION("rootNode has no parent") {
        REQUIRE(backend.parent(backend.rootNode(doc)) == NullNode);
    }

    SECTION("rootNode starts with no children") {
        REQUIRE(backend.children(backend.rootNode(doc)).empty());
    }

    SECTION("rootNode is stable across multiple calls") {
        NodeId r1 = backend.rootNode(doc);
        NodeId r2 = backend.rootNode(doc);
        REQUIRE(r1 == r2);
    }
}

TEST_CASE("newChild", "[persistence][nodes]") {
    NodeFixture f;

    SECTION("newChild returns a non-null id") {
        NodeId c = NullNode;
        f.txn([&] { c = f.backend.newChild(f.root); });
        REQUIRE(c != NullNode);
    }

    SECTION("two siblings have distinct ids") {
        NodeId c1 = f.addChild(f.root);
        NodeId c2 = f.addChild(f.root);
        REQUIRE(c1 != c2);
    }

    SECTION("parent of a child is the root") {
        NodeId c = f.addChild(f.root);
        REQUIRE(f.backend.parent(c) == f.root);
    }

    SECTION("newly created child has no children") {
        NodeId c = f.addChild(f.root);
        REQUIRE(f.backend.children(c).empty());
    }
}

TEST_CASE("children insertion order", "[persistence][nodes]") {
    NodeFixture f;

    NodeId c1 = f.addChild(f.root, "A");
    NodeId c2 = f.addChild(f.root, "B");
    NodeId c3 = f.addChild(f.root, "C");

    auto kids = f.backend.children(f.root);
    REQUIRE(kids.size() == 3);
    REQUIRE(kids[0] == c1);
    REQUIRE(kids[1] == c2);
    REQUIRE(kids[2] == c3);
}

TEST_CASE("deleteNode", "[persistence][nodes]") {
    NodeFixture f;

    SECTION("deleted node is removed from parent's children") {
        NodeId c = f.addChild(f.root);
        f.txn([&] { f.backend.deleteNode(c); });
        REQUIRE(f.backend.children(f.root).empty());
    }

    SECTION("deleting one sibling leaves the others") {
        NodeId c1 = f.addChild(f.root, "keep");
        NodeId c2 = f.addChild(f.root, "drop");
        f.txn([&] { f.backend.deleteNode(c2); });
        auto kids = f.backend.children(f.root);
        REQUIRE(kids.size() == 1);
        REQUIRE(kids[0] == c1);
    }

    SECTION("deleteNode removes descendants recursively") {
        NodeId c  = f.addChild(f.root);
        NodeId gc = f.addChild(c);  // grandchild
        f.addChild(gc);             // great-grandchild
        f.txn([&] { f.backend.deleteNode(c); });
        REQUIRE(f.backend.children(f.root).empty());
    }
}

TEST_CASE("reparent", "[persistence][nodes]") {
    NodeFixture f;

    SECTION("moves node to a different parent") {
        NodeId p1 = f.addChild(f.root, "P1");
        NodeId p2 = f.addChild(f.root, "P2");
        NodeId c  = f.addChild(p1, "Child");

        bool ok = f.backend.reparent(c, p2);
        REQUIRE(ok);
        REQUIRE(f.backend.parent(c) == p2);
        REQUIRE(f.backend.children(p1).empty());
        REQUIRE(f.backend.children(p2).size() == 1);
    }

    SECTION("reparent to root") {
        NodeId p = f.addChild(f.root);
        NodeId c = f.addChild(p);
        bool ok  = f.backend.reparent(c, f.root);
        REQUIRE(ok);
        REQUIRE(f.backend.parent(c) == f.root);
        REQUIRE(f.backend.children(f.root).size() == 2);
    }

    SECTION("reparent to self is rejected") {
        NodeId c = f.addChild(f.root);
        REQUIRE_FALSE(f.backend.reparent(c, c));
    }

    SECTION("reparent to direct descendant is rejected (cycle)") {
        NodeId c  = f.addChild(f.root);
        NodeId gc = f.addChild(c);
        REQUIRE_FALSE(f.backend.reparent(c, gc));
    }

    SECTION("reparent to deep descendant is rejected (cycle)") {
        NodeId c   = f.addChild(f.root);
        NodeId gc  = f.addChild(c);
        NodeId ggc = f.addChild(gc);
        REQUIRE_FALSE(f.backend.reparent(c, ggc));
    }

    SECTION("rejected reparent leaves tree unchanged") {
        NodeId c  = f.addChild(f.root);
        NodeId gc = f.addChild(c);
        f.backend.reparent(c, gc);  // rejected
        REQUIRE(f.backend.parent(c)  == f.root);
        REQUIRE(f.backend.parent(gc) == c);
    }
}

TEST_CASE("deep node tree", "[persistence][nodes]") {
    NodeFixture f;
    const int depth = 30;

    NodeId cur = f.root;
    for (int i = 0; i < depth; ++i)
        cur = f.addChild(cur);

    // Walk up the ancestor chain and count levels.
    int count = 0;
    NodeId n = cur;
    while (n != NullNode) {
        n = f.backend.parent(n);
        ++count;
    }
    REQUIRE(count == depth + 1);  // depth children + root
}
