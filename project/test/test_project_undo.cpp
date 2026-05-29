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

TEST_CASE("Project undo: initial state", "[project][undo]") {
    ProjectFixture f;

    SECTION("canUndo is false on a fresh project") {
        REQUIRE_FALSE(f.project.canUndo());
    }
    SECTION("canRedo is false on a fresh project") {
        REQUIRE_FALSE(f.project.canRedo());
    }
}

// ─── addRobot ─────────────────────────────────────────────────────────────────

TEST_CASE("Project undo: addRobot", "[project][undo]") {
    ProjectFixture f;

    SECTION("canUndo is true after addRobot") {
        f.project.addRobot("UR10e", "robots/ur10e.urdf");
        REQUIRE(f.project.canUndo());
    }

    SECTION("undo removes the added robot") {
        f.project.addRobot("UR10e", "robots/ur10e.urdf");
        f.project.undo();
        REQUIRE(f.project.robots().empty());
    }

    SECTION("canRedo is true after undo") {
        f.project.addRobot("UR10e", "robots/ur10e.urdf");
        f.project.undo();
        REQUIRE(f.project.canRedo());
    }

    SECTION("redo restores the robot") {
        f.project.addRobot("UR10e", "robots/ur10e.urdf");
        f.project.undo();
        f.project.redo();
        REQUIRE(f.project.robots().size() == 1);
    }
}

// ─── addWorkpiece ─────────────────────────────────────────────────────────────

TEST_CASE("Project undo: addWorkpiece", "[project][undo]") {
    ProjectFixture f;

    SECTION("undo removes the added workpiece") {
        f.project.addWorkpiece("PartA", "parts/a.step");
        f.project.undo();
        REQUIRE(f.project.workpieces().empty());
    }
}

// ─── addFixture ───────────────────────────────────────────────────────────────

TEST_CASE("Project undo: addFixture", "[project][undo]") {
    ProjectFixture f;

    SECTION("undo removes the added fixture") {
        f.project.addFixture("Table", "fixtures/t.step");
        f.project.undo();
        REQUIRE(f.project.fixtures().empty());
    }
}

// ─── addProgram ───────────────────────────────────────────────────────────────

TEST_CASE("Project undo: addProgram", "[project][undo]") {
    ProjectFixture f;
    NodeId robot = f.project.addRobot("UR10e", "robots/ur10e.urdf");

    SECTION("undo removes the added program") {
        f.project.addProgram("Weld1", robot);
        f.project.undo();
        REQUIRE(f.project.programs().empty());
    }
}

// ─── addOperation ─────────────────────────────────────────────────────────────

TEST_CASE("Project undo: addOperation", "[project][undo]") {
    ProjectFixture f;
    NodeId robot = f.project.addRobot("UR10e", "robots/ur10e.urdf");
    NodeId prog  = f.project.addProgram("P1", robot);

    SECTION("undo removes the added operation") {
        f.project.addOperation(prog, "Home", "MoveJ");
        REQUIRE(f.project.operations(prog).size() == 1);
        f.project.undo();
        REQUIRE(f.project.operations(prog).empty());
    }
}

// ─── renameItem ───────────────────────────────────────────────────────────────

TEST_CASE("Project undo: renameItem", "[project][undo]") {
    ProjectFixture f;
    NodeId id = f.project.addRobot("OriginalName", "robots/r.urdf");

    SECTION("undo reverts the rename") {
        f.project.renameItem(id, "NewName");
        REQUIRE(f.project.itemName(id) == "NewName");
        f.project.undo();
        REQUIRE(f.project.itemName(id) == "OriginalName");
    }

    SECTION("redo re-applies the rename") {
        f.project.renameItem(id, "NewName");
        f.project.undo();
        f.project.redo();
        REQUIRE(f.project.itemName(id) == "NewName");
    }
}

// ─── setProperty ──────────────────────────────────────────────────────────────

TEST_CASE("Project undo: setProperty", "[project][undo]") {
    ProjectFixture f;
    NodeId robot = f.project.addRobot("UR10e", "robots/r.urdf");
    NodeId prog  = f.project.addProgram("P1", robot);
    NodeId op    = f.project.addOperation(prog, "Op1", "MoveJ");

    SECTION("undo removes a set property") {
        f.project.setProperty(op, "joint_config", "[0,0,0,0,0,0]");
        f.project.undo();
        REQUIRE_FALSE(f.project.getProperty(op, "joint_config").has_value());
    }
}

// ─── Multiple steps ───────────────────────────────────────────────────────────

TEST_CASE("Project undo: multiple steps", "[project][undo]") {
    ProjectFixture f;

    f.project.addRobot("R1", "robots/r1.urdf");
    f.project.addRobot("R2", "robots/r2.urdf");
    f.project.addRobot("R3", "robots/r3.urdf");
    REQUIRE(f.project.robots().size() == 3);

    SECTION("undo walks back one step at a time") {
        f.project.undo();
        REQUIRE(f.project.robots().size() == 2);
        f.project.undo();
        REQUIRE(f.project.robots().size() == 1);
        f.project.undo();
        REQUIRE(f.project.robots().empty());
        REQUIRE_FALSE(f.project.canUndo());
    }

    SECTION("redo walks forward one step at a time") {
        f.project.undo();
        f.project.undo();
        f.project.undo();
        f.project.redo();
        REQUIRE(f.project.robots().size() == 1);
        f.project.redo();
        REQUIRE(f.project.robots().size() == 2);
        f.project.redo();
        REQUIRE(f.project.robots().size() == 3);
        REQUIRE_FALSE(f.project.canRedo());
    }
}

// ─── New mutation clears redo stack ───────────────────────────────────────────

TEST_CASE("Project undo: new mutation clears redo stack", "[project][undo]") {
    ProjectFixture f;

    f.project.addRobot("R1", "robots/r1.urdf");
    f.project.undo();
    REQUIRE(f.project.canRedo());

    f.project.addRobot("R2", "robots/r2.urdf");
    REQUIRE_FALSE(f.project.canRedo());
}
