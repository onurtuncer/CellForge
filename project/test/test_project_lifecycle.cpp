// ------------------------------------------------------------------------------------
// Project: CellForge
// Copyright (C) 2026, Melina Aero Teknoloji Gelistirme ve Dizayn Burosu A.S., Istanbul
// Author: Onur Tuncer, PhD
//
// SPDX-License-Identifier: LGPL-2.1-only
// License-Filename: LICENSE
// ------------------------------------------------------------------------------------

#include <catch2/catch_test_macros.hpp>

#include "test_helpers.h"

using namespace CellForge;

// ─── Initial state ────────────────────────────────────────────────────────────

TEST_CASE("Project: initial state before any call", "[project][lifecycle]") {
    OcafPersistenceBackend backend;
    Project project(backend);

    SECTION("isOpen is false") { REQUIRE_FALSE(project.isOpen()); }
    SECTION("name is empty")   { REQUIRE(project.name().empty()); }
    SECTION("path is empty")   { REQUIRE(project.path().empty()); }
}

// ─── newProject ───────────────────────────────────────────────────────────────

TEST_CASE("Project: newProject", "[project][lifecycle]") {
    OcafPersistenceBackend backend;
    Project project(backend);
    project.newProject("WeldCell");

    SECTION("isOpen is true")             { REQUIRE(project.isOpen()); }
    SECTION("name returns the given name") { REQUIRE(project.name() == "WeldCell"); }
    SECTION("path is empty (unsaved)")    { REQUIRE(project.path().empty()); }

    SECTION("all container nodes are valid") {
        REQUIRE(project.rootNode()        != NullNode);
        REQUIRE(project.robotsNode()      != NullNode);
        REQUIRE(project.workpiecesNode()  != NullNode);
        REQUIRE(project.fixturesNode()    != NullNode);
        REQUIRE(project.programsNode()    != NullNode);
    }

    SECTION("all item lists are empty") {
        REQUIRE(project.robots().empty());
        REQUIRE(project.workpieces().empty());
        REQUIRE(project.fixtures().empty());
        REQUIRE(project.programs().empty());
    }
}

TEST_CASE("Project: newProject replaces an already-open project", "[project][lifecycle]") {
    OcafPersistenceBackend backend;
    Project project(backend);

    project.newProject("First");
    NodeId firstRoot = project.rootNode();

    project.newProject("Second");
    REQUIRE(project.name() == "Second");
    REQUIRE(project.rootNode() != firstRoot);
    REQUIRE(project.robots().empty());
}

// ─── close ────────────────────────────────────────────────────────────────────

TEST_CASE("Project: close", "[project][lifecycle]") {
    OcafPersistenceBackend backend;
    Project project(backend);

    SECTION("close on an open project makes isOpen false") {
        project.newProject("Temp");
        project.close();
        REQUIRE_FALSE(project.isOpen());
    }

    SECTION("close on an already-closed project is a no-op") {
        REQUIRE_NOTHROW(project.close());
    }

    SECTION("name and path are empty after close") {
        project.newProject("Temp");
        project.close();
        REQUIRE(project.name().empty());
        REQUIRE(project.path().empty());
    }
}

// ─── save / open ──────────────────────────────────────────────────────────────

TEST_CASE("Project: saveAs", "[project][lifecycle]") {
    OcafPersistenceBackend backend;
    TempFile tmp("cf_proj_lifecycle.cbf");

    SECTION("saveAs writes the file and updates path") {
        Project project(backend);
        project.newProject("SaveMe");
        REQUIRE(project.saveAs(tmp.path.string()));
        REQUIRE(project.path() == tmp.path.string());
        REQUIRE(fs::exists(tmp.path));
    }

    SECTION("isModified clears after saveAs") {
        Project project(backend);
        project.newProject("Dirty");
        project.addRobot("UR10e", "robots/ur10e.urdf");
        REQUIRE(project.isModified());
        project.saveAs(tmp.path.string());
        REQUIRE_FALSE(project.isModified());
    }

    SECTION("saveAs returns false on a closed project") {
        Project project(backend);
        REQUIRE_FALSE(project.saveAs(tmp.path.string()));
    }
}

TEST_CASE("Project: save", "[project][lifecycle]") {
    OcafPersistenceBackend backend;
    TempFile tmp("cf_proj_save.cbf");

    SECTION("save returns false when no path is set") {
        Project project(backend);
        project.newProject("NoPath");
        REQUIRE_FALSE(project.save());
    }

    SECTION("save succeeds after saveAs has set the path") {
        Project project(backend);
        project.newProject("WithPath");
        project.saveAs(tmp.path.string());
        REQUIRE(project.save());
    }
}

TEST_CASE("Project: open", "[project][lifecycle]") {
    OcafPersistenceBackend backend;
    TempFile tmp("cf_proj_open.cbf");

    SECTION("open returns false for a missing file") {
        Project project(backend);
        REQUIRE_FALSE(project.open("no_such_file.cbf"));
        REQUIRE_FALSE(project.isOpen());
    }

    SECTION("open sets the path") {
        { Project w(backend); w.newProject("PathTest"); w.saveAs(tmp.path.string()); }
        Project reader(backend);
        REQUIRE(reader.open(tmp.path.string()));
        REQUIRE(reader.path() == tmp.path.string());
    }

    SECTION("open makes isOpen true") {
        { Project w(backend); w.newProject("X"); w.saveAs(tmp.path.string()); }
        Project reader(backend);
        REQUIRE(reader.open(tmp.path.string()));
        REQUIRE(reader.isOpen());
    }

    SECTION("open restores the project name") {
        { Project w(backend); w.newProject("MyCell"); w.saveAs(tmp.path.string()); }
        Project reader(backend);
        reader.open(tmp.path.string());
        REQUIRE(reader.name() == "MyCell");
    }
}
