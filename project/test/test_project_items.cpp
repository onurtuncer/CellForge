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

#include "test_helpers.h"

using namespace CellForge;

// ─── Robots ───────────────────────────────────────────────────────────────────

TEST_CASE("Project: add and query robots", "[project][items][robot]") {
    ProjectFixture f;

    SECTION("addRobot returns a non-null id") {
        REQUIRE(f.project.addRobot("UR10e", "robots/ur10e.urdf") != NullNode);
    }

    SECTION("added robot appears in robots()") {
        NodeId id = f.project.addRobot("UR10e", "robots/ur10e.urdf");
        auto list = f.project.robots();
        REQUIRE(list.size() == 1);
        REQUIRE(list[0] == id);
    }

    SECTION("multiple robots are all returned in insertion order") {
        NodeId r1 = f.project.addRobot("UR10e", "robots/ur10e.urdf");
        NodeId r2 = f.project.addRobot("KR210", "robots/kr210.urdf");
        NodeId r3 = f.project.addRobot("M20iB", "robots/m20ib.urdf");
        auto list = f.project.robots();
        REQUIRE(list.size() == 3);
        REQUIRE(list[0] == r1);
        REQUIRE(list[1] == r2);
        REQUIRE(list[2] == r3);
    }

    SECTION("itemType is Robot") {
        NodeId id = f.project.addRobot("UR10e", "robots/ur10e.urdf");
        REQUIRE(f.project.itemType(id) == "Robot");
    }

    SECTION("itemName returns the given name") {
        NodeId id = f.project.addRobot("UR10e", "robots/ur10e.urdf");
        REQUIRE(f.project.itemName(id) == "UR10e");
    }

    SECTION("robotUrdfPath returns the URDF path") {
        NodeId id = f.project.addRobot("UR10e", "robots/ur10e.urdf");
        REQUIRE(f.project.robotUrdfPath(id) == "robots/ur10e.urdf");
    }

    SECTION("robotSrdfPath is empty when not provided") {
        NodeId id = f.project.addRobot("UR10e", "robots/ur10e.urdf");
        REQUIRE(f.project.robotSrdfPath(id).empty());
    }

    SECTION("robotSrdfPath returns the SRDF path when provided") {
        NodeId id = f.project.addRobot("UR10e", "robots/ur10e.urdf", "robots/ur10e.srdf");
        REQUIRE(f.project.robotSrdfPath(id) == "robots/ur10e.srdf");
    }
}

TEST_CASE("Project: remove robot", "[project][items][robot]") {
    ProjectFixture f;

    SECTION("removeRobot empties the list") {
        NodeId id = f.project.addRobot("UR10e", "robots/ur10e.urdf");
        f.project.removeRobot(id);
        REQUIRE(f.project.robots().empty());
    }

    SECTION("removing one robot leaves the others") {
        NodeId r1 = f.project.addRobot("UR10e", "robots/ur10e.urdf");
        NodeId r2 = f.project.addRobot("KR210", "robots/kr210.urdf");
        f.project.removeRobot(r1);
        auto list = f.project.robots();
        REQUIRE(list.size() == 1);
        REQUIRE(list[0] == r2);
    }
}

// ─── Workpieces ───────────────────────────────────────────────────────────────

TEST_CASE("Project: workpieces", "[project][items][workpiece]") {
    ProjectFixture f;

    SECTION("addWorkpiece returns a non-null id") {
        REQUIRE(f.project.addWorkpiece("PartA", "parts/a.step") != NullNode);
    }

    SECTION("workpiece appears in workpieces()") {
        NodeId id = f.project.addWorkpiece("PartA", "parts/a.step");
        REQUIRE(f.project.workpieces().size() == 1);
        REQUIRE(f.project.workpieces()[0] == id);
    }

    SECTION("itemType is Workpiece") {
        NodeId id = f.project.addWorkpiece("PartA", "parts/a.step");
        REQUIRE(f.project.itemType(id) == "Workpiece");
    }

    SECTION("removeWorkpiece empties the list") {
        NodeId id = f.project.addWorkpiece("PartA", "parts/a.step");
        f.project.removeWorkpiece(id);
        REQUIRE(f.project.workpieces().empty());
    }
}

// ─── Fixtures ─────────────────────────────────────────────────────────────────

TEST_CASE("Project: fixtures", "[project][items][fixture]") {
    ProjectFixture f;

    SECTION("addFixture returns a non-null id") {
        REQUIRE(f.project.addFixture("Table", "fixtures/table.step") != NullNode);
    }

    SECTION("fixture appears in fixtures()") {
        NodeId id = f.project.addFixture("Table", "fixtures/table.step");
        REQUIRE(f.project.fixtures().size() == 1);
        REQUIRE(f.project.fixtures()[0] == id);
    }

    SECTION("itemType is Fixture") {
        NodeId id = f.project.addFixture("Table", "fixtures/table.step");
        REQUIRE(f.project.itemType(id) == "Fixture");
    }

    SECTION("removeFixture empties the list") {
        NodeId id = f.project.addFixture("Table", "fixtures/table.step");
        f.project.removeFixture(id);
        REQUIRE(f.project.fixtures().empty());
    }
}

// ─── Programs ─────────────────────────────────────────────────────────────────

TEST_CASE("Project: programs", "[project][items][program]") {
    ProjectFixture f;
    NodeId robot = f.project.addRobot("UR10e", "robots/ur10e.urdf");

    SECTION("addProgram returns a non-null id") {
        REQUIRE(f.project.addProgram("Weld1", robot) != NullNode);
    }

    SECTION("program appears in programs()") {
        NodeId prog = f.project.addProgram("Weld1", robot);
        REQUIRE(f.project.programs().size() == 1);
        REQUIRE(f.project.programs()[0] == prog);
    }

    SECTION("itemType is Program") {
        NodeId prog = f.project.addProgram("Weld1", robot);
        REQUIRE(f.project.itemType(prog) == "Program");
    }

    SECTION("robotForProgram returns the assigned robot id") {
        NodeId prog = f.project.addProgram("Weld1", robot);
        REQUIRE(f.project.robotForProgram(prog) == robot);
    }

    SECTION("multiple programs can reference different robots") {
        NodeId r2   = f.project.addRobot("KR210", "robots/kr210.urdf");
        NodeId p1   = f.project.addProgram("ProgA", robot);
        NodeId p2   = f.project.addProgram("ProgB", r2);
        REQUIRE(f.project.robotForProgram(p1) == robot);
        REQUIRE(f.project.robotForProgram(p2) == r2);
    }

    SECTION("removeProgram removes it and its operations") {
        NodeId prog = f.project.addProgram("Weld1", robot);
        f.project.addOperation(prog, "Home", "MoveJ");
        f.project.removeProgram(prog);
        REQUIRE(f.project.programs().empty());
    }
}

// ─── Operations ───────────────────────────────────────────────────────────────

TEST_CASE("Project: operations", "[project][items][operation]") {
    ProjectFixture f;
    NodeId robot = f.project.addRobot("UR10e", "robots/ur10e.urdf");
    NodeId prog  = f.project.addProgram("P1", robot);

    SECTION("addOperation returns a non-null id") {
        REQUIRE(f.project.addOperation(prog, "Home", "MoveJ") != NullNode);
    }

    SECTION("operation appears in operations()") {
        NodeId op = f.project.addOperation(prog, "Home", "MoveJ");
        auto list = f.project.operations(prog);
        REQUIRE(list.size() == 1);
        REQUIRE(list[0] == op);
    }

    SECTION("itemType is Operation") {
        NodeId op = f.project.addOperation(prog, "Home", "MoveJ");
        REQUIRE(f.project.itemType(op) == "Operation");
    }

    SECTION("operationType returns the given type string") {
        NodeId op = f.project.addOperation(prog, "Approach", "MoveL");
        REQUIRE(f.project.operationType(op) == "MoveL");
    }

    SECTION("multiple operations preserve insertion order") {
        NodeId op1 = f.project.addOperation(prog, "Home",     "MoveJ");
        NodeId op2 = f.project.addOperation(prog, "Approach", "MoveL");
        NodeId op3 = f.project.addOperation(prog, "Weld",     "MoveL");
        auto list = f.project.operations(prog);
        REQUIRE(list.size() == 3);
        REQUIRE(list[0] == op1);
        REQUIRE(list[1] == op2);
        REQUIRE(list[2] == op3);
    }

    SECTION("removeOperation removes it from the list") {
        NodeId op = f.project.addOperation(prog, "Home", "MoveJ");
        f.project.removeOperation(op);
        REQUIRE(f.project.operations(prog).empty());
    }

    SECTION("operations are scoped to their program") {
        NodeId prog2 = f.project.addProgram("P2", robot);
        f.project.addOperation(prog,  "OpA", "MoveJ");
        f.project.addOperation(prog2, "OpB", "MoveL");
        REQUIRE(f.project.operations(prog).size()  == 1);
        REQUIRE(f.project.operations(prog2).size() == 1);
    }

    SECTION("an empty program has no operations") {
        NodeId prog2 = f.project.addProgram("Empty", robot);
        REQUIRE(f.project.operations(prog2).empty());
    }
}

// ─── Generic item access ──────────────────────────────────────────────────────

TEST_CASE("Project: generic item access", "[project][items]") {
    ProjectFixture f;

    SECTION("renameItem updates itemName") {
        NodeId id = f.project.addRobot("OldName", "robots/r.urdf");
        f.project.renameItem(id, "NewName");
        REQUIRE(f.project.itemName(id) == "NewName");
    }

    SECTION("setItemTransform and itemTransform round-trip in memory") {
        NodeId id = f.project.addWorkpiece("Part", "parts/p.step");
        Transform t;
        t(0, 3) = 1.0;
        t(1, 3) = 2.0;
        t(2, 3) = 3.0;
        f.project.setItemTransform(id, t);
        Transform r = f.project.itemTransform(id);
        for (int i = 0; i < 16; ++i)
            REQUIRE(r.data[i] == Catch::Approx(t.data[i]));
    }

    SECTION("itemTransform returns identity by default") {
        NodeId id = f.project.addFixture("Table", "fixtures/t.step");
        Transform identity = Transform::identity();
        Transform r        = f.project.itemTransform(id);
        for (int i = 0; i < 16; ++i)
            REQUIRE(r.data[i] == Catch::Approx(identity.data[i]));
    }

    SECTION("setProperty and getProperty round-trip") {
        NodeId robot = f.project.addRobot("UR10e", "robots/r.urdf");
        NodeId prog  = f.project.addProgram("P1", robot);
        NodeId op    = f.project.addOperation(prog, "Op1", "MoveJ");
        f.project.setProperty(op, "joint_config", "[0,1.57,0,0,0,0]");
        REQUIRE(f.project.getProperty(op, "joint_config") == "[0,1.57,0,0,0,0]");
    }

    SECTION("getProperty returns nullopt for an absent key") {
        NodeId id = f.project.addRobot("UR10e", "robots/r.urdf");
        REQUIRE_FALSE(f.project.getProperty(id, "nonexistent").has_value());
    }

    SECTION("isModified becomes true after adding an item") {
        f.project.addRobot("UR10e", "robots/r.urdf");
        REQUIRE(f.project.isModified());
    }
}
