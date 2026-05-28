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

using namespace CellForge;

// ─── Fixture ──────────────────────────────────────────────────────────────────

struct UndoFixture {
    OcafPersistenceBackend backend;
    DocumentId doc;
    NodeId root;

    UndoFixture() {
        doc  = backend.newDocument("UndoTest");
        root = backend.rootNode(doc);
    }

    template<typename Fn>
    NodeId txn(std::string_view label, Fn fn) {
        backend.beginTransaction(doc, label);
        NodeId result = NullNode;
        fn(result);
        backend.commitTransaction(doc);
        return result;
    }
};

// ─── Initial state ────────────────────────────────────────────────────────────

TEST_CASE("Undo/Redo: initial state", "[persistence][undo]") {
    OcafPersistenceBackend backend;
    DocumentId doc = backend.newDocument("Fresh");

    SECTION("canUndo is false on a fresh document") {
        REQUIRE_FALSE(backend.canUndo(doc));
    }

    SECTION("canRedo is false on a fresh document") {
        REQUIRE_FALSE(backend.canRedo(doc));
    }

    SECTION("commitTransaction returns false when no transaction is open") {
        REQUIRE_FALSE(backend.commitTransaction(doc));
    }
}

// ─── Commit creates an undo step ─────────────────────────────────────────────

TEST_CASE("Undo/Redo: commit", "[persistence][undo]") {
    UndoFixture f;

    SECTION("canUndo is true after a commit") {
        f.backend.beginTransaction(f.doc, "add");
        f.backend.newChild(f.root);
        f.backend.commitTransaction(f.doc);
        REQUIRE(f.backend.canUndo(f.doc));
    }

    SECTION("undo reverts a child addition") {
        f.backend.beginTransaction(f.doc, "add");
        f.backend.newChild(f.root);
        f.backend.commitTransaction(f.doc);

        REQUIRE(f.backend.children(f.root).size() == 1);
        f.backend.undo(f.doc);
        REQUIRE(f.backend.children(f.root).empty());
    }

    SECTION("canRedo is true after undo") {
        f.backend.beginTransaction(f.doc, "add");
        f.backend.newChild(f.root);
        f.backend.commitTransaction(f.doc);
        f.backend.undo(f.doc);
        REQUIRE(f.backend.canRedo(f.doc));
    }

    SECTION("redo re-applies the undone change") {
        f.backend.beginTransaction(f.doc, "add");
        f.backend.newChild(f.root);
        f.backend.commitTransaction(f.doc);
        f.backend.undo(f.doc);
        f.backend.redo(f.doc);
        REQUIRE(f.backend.children(f.root).size() == 1);
    }
}

// ─── Abort rolls back without adding a step ──────────────────────────────────

TEST_CASE("Undo/Redo: abort", "[persistence][undo]") {
    UndoFixture f;

    SECTION("abortTransaction discards changes") {
        f.backend.beginTransaction(f.doc, "attempt");
        f.backend.newChild(f.root);
        f.backend.abortTransaction(f.doc);
        REQUIRE(f.backend.children(f.root).empty());
    }

    SECTION("aborted transaction does not appear on the undo stack") {
        f.backend.beginTransaction(f.doc, "attempt");
        f.backend.newChild(f.root);
        f.backend.abortTransaction(f.doc);
        REQUIRE_FALSE(f.backend.canUndo(f.doc));
    }
}

// ─── Undo reverts attribute changes ──────────────────────────────────────────

TEST_CASE("Undo/Redo: attribute changes", "[persistence][undo]") {
    UndoFixture f;

    // Add a node to set attributes on.
    f.backend.beginTransaction(f.doc, "add node");
    NodeId n = f.backend.newChild(f.root);
    f.backend.commitTransaction(f.doc);

    SECTION("undo reverts setReal") {
        f.backend.beginTransaction(f.doc, "set real");
        f.backend.setReal(n, "val", 99.0);
        f.backend.commitTransaction(f.doc);

        REQUIRE(f.backend.getReal(n, "val").has_value());
        f.backend.undo(f.doc);
        REQUIRE_FALSE(f.backend.getReal(n, "val").has_value());
    }

    SECTION("undo reverts setInt") {
        f.backend.beginTransaction(f.doc, "set int");
        f.backend.setInt(n, "count", 42LL);
        f.backend.commitTransaction(f.doc);

        f.backend.undo(f.doc);
        REQUIRE_FALSE(f.backend.getInt(n, "count").has_value());
    }

    SECTION("undo reverts setBool") {
        f.backend.beginTransaction(f.doc, "set bool");
        f.backend.setBool(n, "flag", true);
        f.backend.commitTransaction(f.doc);

        f.backend.undo(f.doc);
        REQUIRE_FALSE(f.backend.getBool(n, "flag").has_value());
    }

    SECTION("undo reverts setName") {
        f.backend.beginTransaction(f.doc, "set name");
        f.backend.setName(n, "NewName");
        f.backend.commitTransaction(f.doc);

        f.backend.undo(f.doc);
        REQUIRE(f.backend.getName(n) != "NewName");
    }

    SECTION("undo/redo on setReal preserves value on redo") {
        f.backend.beginTransaction(f.doc, "set real");
        f.backend.setReal(n, "v", 3.14);
        f.backend.commitTransaction(f.doc);

        f.backend.undo(f.doc);
        f.backend.redo(f.doc);
        REQUIRE(*f.backend.getReal(n, "v") == Catch::Approx(3.14));
    }
}

// ─── Multiple undo/redo steps ─────────────────────────────────────────────────

TEST_CASE("Undo/Redo: multiple steps", "[persistence][undo]") {
    UndoFixture f;

    // Three separate committed transactions.
    f.backend.beginTransaction(f.doc, "step1");
    f.backend.newChild(f.root);
    f.backend.commitTransaction(f.doc);

    f.backend.beginTransaction(f.doc, "step2");
    f.backend.newChild(f.root);
    f.backend.commitTransaction(f.doc);

    f.backend.beginTransaction(f.doc, "step3");
    f.backend.newChild(f.root);
    f.backend.commitTransaction(f.doc);

    REQUIRE(f.backend.children(f.root).size() == 3);

    SECTION("undo walks back one step at a time") {
        f.backend.undo(f.doc);
        REQUIRE(f.backend.children(f.root).size() == 2);
        f.backend.undo(f.doc);
        REQUIRE(f.backend.children(f.root).size() == 1);
        f.backend.undo(f.doc);
        REQUIRE(f.backend.children(f.root).empty());
        REQUIRE_FALSE(f.backend.canUndo(f.doc));
    }

    SECTION("redo walks forward one step at a time") {
        f.backend.undo(f.doc);
        f.backend.undo(f.doc);
        f.backend.undo(f.doc);

        f.backend.redo(f.doc);
        REQUIRE(f.backend.children(f.root).size() == 1);
        f.backend.redo(f.doc);
        REQUIRE(f.backend.children(f.root).size() == 2);
        f.backend.redo(f.doc);
        REQUIRE(f.backend.children(f.root).size() == 3);
        REQUIRE_FALSE(f.backend.canRedo(f.doc));
    }
}

// ─── New action clears redo stack ────────────────────────────────────────────

TEST_CASE("Undo/Redo: new mutation clears redo", "[persistence][undo]") {
    UndoFixture f;

    f.backend.beginTransaction(f.doc, "add");
    f.backend.newChild(f.root);
    f.backend.commitTransaction(f.doc);

    f.backend.undo(f.doc);
    REQUIRE(f.backend.canRedo(f.doc));

    f.backend.beginTransaction(f.doc, "new action");
    f.backend.newChild(f.root);
    f.backend.commitTransaction(f.doc);

    REQUIRE_FALSE(f.backend.canRedo(f.doc));
}

// ─── setUndoLimit ────────────────────────────────────────────────────────────

TEST_CASE("Undo/Redo: setUndoLimit", "[persistence][undo]") {
    UndoFixture f;

    SECTION("limit of 1 retains only the most recent step") {
        f.backend.setUndoLimit(f.doc, 1);

        f.backend.beginTransaction(f.doc, "step1");
        f.backend.newChild(f.root);
        f.backend.commitTransaction(f.doc);

        f.backend.beginTransaction(f.doc, "step2");
        f.backend.newChild(f.root);
        f.backend.commitTransaction(f.doc);

        REQUIRE(f.backend.children(f.root).size() == 2);

        // The most recent step (step2) should be undoable.
        REQUIRE(f.backend.canUndo(f.doc));
        f.backend.undo(f.doc);
        REQUIRE(f.backend.children(f.root).size() == 1);

        // step1 was evicted from the undo stack.
        REQUIRE_FALSE(f.backend.canUndo(f.doc));
    }

    SECTION("limit of 0 means unlimited") {
        f.backend.setUndoLimit(f.doc, 0);

        const int steps = 10;
        for (int i = 0; i < steps; ++i) {
            f.backend.beginTransaction(f.doc, "step");
            f.backend.newChild(f.root);
            f.backend.commitTransaction(f.doc);
        }

        int undoCount = 0;
        while (f.backend.canUndo(f.doc)) {
            f.backend.undo(f.doc);
            ++undoCount;
        }
        REQUIRE(undoCount == steps);
        REQUIRE(f.backend.children(f.root).empty());
    }
}
