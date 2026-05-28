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

#include <filesystem>
#include <string>

using namespace CellForge;
namespace fs = std::filesystem;

// ─── Helpers ──────────────────────────────────────────────────────────────────

struct TempFile {
    fs::path path;
    explicit TempFile(std::string_view name = "cellforge_test.cbf")
        : path(fs::temp_directory_path() / name) {}
    ~TempFile() { std::error_code ec; fs::remove(path, ec); }
};

// ─── Tests ────────────────────────────────────────────────────────────────────

TEST_CASE("Document lifecycle", "[persistence][document]") {
    OcafPersistenceBackend backend;

    SECTION("newDocument returns a non-null id") {
        DocumentId doc = backend.newDocument("Test");
        REQUIRE(doc != NullDocument);
    }

    SECTION("multiple documents receive unique ids") {
        DocumentId d1 = backend.newDocument("A");
        DocumentId d2 = backend.newDocument("B");
        DocumentId d3 = backend.newDocument("C");
        REQUIRE(d1 != d2);
        REQUIRE(d2 != d3);
        REQUIRE(d1 != d3);
    }

    SECTION("closeDocument does not throw on a valid id") {
        DocumentId doc = backend.newDocument("Close");
        REQUIRE_NOTHROW(backend.closeDocument(doc));
    }

    SECTION("fresh document is not modified") {
        DocumentId doc = backend.newDocument("Fresh");
        REQUIRE_FALSE(backend.isModified(doc));
    }

    SECTION("isModified becomes true after a committed mutation") {
        DocumentId doc  = backend.newDocument("Dirty");
        NodeId     root = backend.rootNode(doc);
        backend.beginTransaction(doc, "add");
        backend.newChild(root);
        backend.commitTransaction(doc);
        REQUIRE(backend.isModified(doc));
    }

    SECTION("openDocument returns NullDocument for a missing file") {
        DocumentId doc = backend.openDocument("no_such_file.cbf");
        REQUIRE(doc == NullDocument);
    }
}

TEST_CASE("Document save and load", "[persistence][document]") {
    OcafPersistenceBackend backend;
    TempFile tmp("cf_save_test.cbf");

    SECTION("saveDocumentAs creates the file on disk") {
        DocumentId doc = backend.newDocument("SaveMe");
        bool ok = backend.saveDocumentAs(doc, tmp.path.string());
        REQUIRE(ok);
        REQUIRE(fs::exists(tmp.path));
    }

    SECTION("isModified clears after saveDocumentAs") {
        DocumentId doc  = backend.newDocument("SaveClear");
        NodeId     root = backend.rootNode(doc);
        backend.beginTransaction(doc, "mutate");
        backend.newChild(root);
        backend.commitTransaction(doc);

        REQUIRE(backend.isModified(doc));
        backend.saveDocumentAs(doc, tmp.path.string());
        REQUIRE_FALSE(backend.isModified(doc));
    }

    SECTION("round-trip preserves node tree structure") {
        {
            DocumentId doc  = backend.newDocument("RT");
            NodeId     root = backend.rootNode(doc);
            backend.beginTransaction(doc, "setup");
            NodeId c1 = backend.newChild(root);
            NodeId c2 = backend.newChild(root);
            backend.newChild(c1);  // grandchild
            backend.setName(c1, "ChildA");
            backend.setName(c2, "ChildB");
            backend.commitTransaction(doc);
            REQUIRE(backend.saveDocumentAs(doc, tmp.path.string()));
            backend.closeDocument(doc);
        }

        DocumentId doc2  = backend.openDocument(tmp.path.string());
        REQUIRE(doc2 != NullDocument);

        NodeId root2 = backend.rootNode(doc2);
        auto   kids  = backend.children(root2);
        REQUIRE(kids.size() == 2);
        REQUIRE(backend.getName(kids[0]) == "ChildA");
        REQUIRE(backend.getName(kids[1]) == "ChildB");
        REQUIRE(backend.children(kids[0]).size() == 1);
        REQUIRE(backend.children(kids[1]).empty());
    }

    SECTION("round-trip preserves typed attributes") {
        {
            DocumentId doc  = backend.newDocument("Attrs");
            NodeId     root = backend.rootNode(doc);
            backend.beginTransaction(doc, "setup");
            NodeId n = backend.newChild(root);
            backend.setReal(n,   "pi",    3.14159265358979);
            backend.setInt(n,    "count", 12345LL);
            backend.setBool(n,   "valid", true);
            backend.setString(n, "tag",   "hello");
            backend.commitTransaction(doc);
            REQUIRE(backend.saveDocumentAs(doc, tmp.path.string()));
            backend.closeDocument(doc);
        }

        DocumentId doc2 = backend.openDocument(tmp.path.string());
        REQUIRE(doc2 != NullDocument);

        NodeId node = backend.children(backend.rootNode(doc2)).at(0);
        REQUIRE(*backend.getReal(node,   "pi")    == Catch::Approx(3.14159265358979));
        REQUIRE(*backend.getInt(node,    "count") == 12345LL);
        REQUIRE(*backend.getBool(node,   "valid") == true);
        REQUIRE(*backend.getString(node, "tag")   == "hello");
    }

    SECTION("round-trip preserves transform") {
        Transform t;
        t(0, 3) = 1.5;
        t(1, 3) = 2.5;
        t(2, 3) = 3.5;

        {
            DocumentId doc  = backend.newDocument("Trsf");
            NodeId     root = backend.rootNode(doc);
            backend.beginTransaction(doc, "setup");
            NodeId n = backend.newChild(root);
            backend.setTransform(n, t);
            backend.commitTransaction(doc);
            REQUIRE(backend.saveDocumentAs(doc, tmp.path.string()));
            backend.closeDocument(doc);
        }

        DocumentId doc2 = backend.openDocument(tmp.path.string());
        REQUIRE(doc2 != NullDocument);

        NodeId    node = backend.children(backend.rootNode(doc2)).at(0);
        Transform r    = backend.getTransform(node);
        for (int i = 0; i < 16; ++i)
            REQUIRE(r.data[i] == Catch::Approx(t.data[i]));
    }

    SECTION("round-trip preserves shape id") {
        constexpr ShapeId sid = 0xCAFEBABE00000001ULL;

        {
            DocumentId doc  = backend.newDocument("Shape");
            NodeId     root = backend.rootNode(doc);
            backend.beginTransaction(doc, "setup");
            NodeId n = backend.newChild(root);
            backend.setShape(n, sid);
            backend.commitTransaction(doc);
            REQUIRE(backend.saveDocumentAs(doc, tmp.path.string()));
            backend.closeDocument(doc);
        }

        DocumentId doc2 = backend.openDocument(tmp.path.string());
        REQUIRE(doc2 != NullDocument);

        NodeId node = backend.children(backend.rootNode(doc2)).at(0);
        REQUIRE(backend.getShape(node) == sid);
    }
}
