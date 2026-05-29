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

// Each section saves a project to a temp file and reopens it in a fresh Project
// instance, verifying the data survived serialization.

TEST_CASE("Project round-trip: project name", "[project][roundtrip]") {
    OcafPersistenceBackend backend;
    TempFile tmp("cf_rt_name.cbf");

    { Project w(backend); w.newProject("MyCell"); w.saveAs(tmp.path.string()); }

    Project r(backend);
    REQUIRE(r.open(tmp.path.string()));
    REQUIRE(r.name() == "MyCell");
}

TEST_CASE("Project round-trip: robots", "[project][roundtrip]") {
    OcafPersistenceBackend backend;
    TempFile tmp("cf_rt_robots.cbf");

    {
        Project w(backend);
        w.newProject("RobotRT");
        w.addRobot("UR10e", "robots/ur10e.urdf", "robots/ur10e.srdf");
        w.addRobot("KR210", "robots/kr210.urdf");
        w.saveAs(tmp.path.string());
    }

    Project r(backend);
    REQUIRE(r.open(tmp.path.string()));

    auto list = r.robots();
    REQUIRE(list.size() == 2);

    REQUIRE(r.itemName(list[0])      == "UR10e");
    REQUIRE(r.itemType(list[0])      == "Robot");
    REQUIRE(r.robotUrdfPath(list[0]) == "robots/ur10e.urdf");
    REQUIRE(r.robotSrdfPath(list[0]) == "robots/ur10e.srdf");

    REQUIRE(r.itemName(list[1])      == "KR210");
    REQUIRE(r.robotUrdfPath(list[1]) == "robots/kr210.urdf");
    REQUIRE(r.robotSrdfPath(list[1]).empty());
}

TEST_CASE("Project round-trip: workpieces", "[project][roundtrip]") {
    OcafPersistenceBackend backend;
    TempFile tmp("cf_rt_wp.cbf");

    {
        Project w(backend);
        w.newProject("WpRT");
        w.addWorkpiece("PartA", "parts/a.step");
        w.saveAs(tmp.path.string());
    }

    Project r(backend);
    REQUIRE(r.open(tmp.path.string()));

    auto list = r.workpieces();
    REQUIRE(list.size() == 1);
    REQUIRE(r.itemName(list[0]) == "PartA");
    REQUIRE(r.itemType(list[0]) == "Workpiece");
}

TEST_CASE("Project round-trip: fixtures", "[project][roundtrip]") {
    OcafPersistenceBackend backend;
    TempFile tmp("cf_rt_fix.cbf");

    {
        Project w(backend);
        w.newProject("FixRT");
        w.addFixture("Table", "fixtures/table.step");
        w.saveAs(tmp.path.string());
    }

    Project r(backend);
    REQUIRE(r.open(tmp.path.string()));

    auto list = r.fixtures();
    REQUIRE(list.size() == 1);
    REQUIRE(r.itemName(list[0]) == "Table");
    REQUIRE(r.itemType(list[0]) == "Fixture");
}

TEST_CASE("Project round-trip: programs and robot references", "[project][roundtrip]") {
    OcafPersistenceBackend backend;
    TempFile tmp("cf_rt_prog.cbf");

    {
        Project w(backend);
        w.newProject("ProgRT");
        NodeId robot = w.addRobot("UR10e", "robots/ur10e.urdf");
        w.addProgram("Weld1", robot);
        w.saveAs(tmp.path.string());
    }

    Project r(backend);
    REQUIRE(r.open(tmp.path.string()));
    REQUIRE(r.programs().size() == 1);

    NodeId prog  = r.programs()[0];
    NodeId robot = r.robots()[0];
    REQUIRE(r.itemName(prog)        == "Weld1");
    REQUIRE(r.itemType(prog)        == "Program");
    // The stored robot reference must resolve to the robot present in this session.
    REQUIRE(r.robotForProgram(prog) == robot);
}

TEST_CASE("Project round-trip: operations", "[project][roundtrip]") {
    OcafPersistenceBackend backend;
    TempFile tmp("cf_rt_ops.cbf");

    {
        Project w(backend);
        w.newProject("OpRT");
        NodeId robot = w.addRobot("UR10e", "robots/ur10e.urdf");
        NodeId prog  = w.addProgram("P1", robot);
        w.addOperation(prog, "Home",     "MoveJ");
        w.addOperation(prog, "Approach", "MoveL");
        w.saveAs(tmp.path.string());
    }

    Project r(backend);
    REQUIRE(r.open(tmp.path.string()));

    NodeId prog = r.programs()[0];
    auto ops = r.operations(prog);
    REQUIRE(ops.size() == 2);
    REQUIRE(r.itemName(ops[0])      == "Home");
    REQUIRE(r.operationType(ops[0]) == "MoveJ");
    REQUIRE(r.itemName(ops[1])      == "Approach");
    REQUIRE(r.operationType(ops[1]) == "MoveL");
}

TEST_CASE("Project round-trip: item transforms", "[project][roundtrip]") {
    OcafPersistenceBackend backend;
    TempFile tmp("cf_rt_trsf.cbf");

    Transform t;
    t(0, 3) = 1.5;
    t(1, 3) = 2.5;
    t(2, 3) = 3.5;

    {
        Project w(backend);
        w.newProject("TrsfRT");
        NodeId id = w.addWorkpiece("Part", "parts/p.step");
        w.setItemTransform(id, t);
        w.saveAs(tmp.path.string());
    }

    Project r(backend);
    REQUIRE(r.open(tmp.path.string()));
    Transform rt = r.itemTransform(r.workpieces()[0]);
    for (int i = 0; i < 16; ++i)
        REQUIRE(rt.data[i] == Catch::Approx(t.data[i]));
}

TEST_CASE("Project round-trip: custom properties", "[project][roundtrip]") {
    OcafPersistenceBackend backend;
    TempFile tmp("cf_rt_props.cbf");

    {
        Project w(backend);
        w.newProject("PropRT");
        NodeId robot = w.addRobot("UR10e", "robots/ur10e.urdf");
        NodeId prog  = w.addProgram("P1", robot);
        NodeId op    = w.addOperation(prog, "Op1", "MoveJ");
        w.setProperty(op, "joint_config", "[0,1.57,0,0,0,0]");
        w.saveAs(tmp.path.string());
    }

    Project r(backend);
    REQUIRE(r.open(tmp.path.string()));
    NodeId op = r.operations(r.programs()[0])[0];
    REQUIRE(r.getProperty(op, "joint_config") == "[0,1.57,0,0,0,0]");
}

TEST_CASE("Project round-trip: full multi-robot cell", "[project][roundtrip]") {
    OcafPersistenceBackend backend;
    TempFile tmp("cf_rt_full.cbf");

    {
        Project w(backend);
        w.newProject("MultiRobot");
        NodeId r1 = w.addRobot("UR10e",  "robots/ur10e.urdf");
        NodeId r2 = w.addRobot("KR210",  "robots/kr210.urdf");
        w.addWorkpiece("Frame",       "parts/frame.step");
        w.addFixture("Positioner",    "fixtures/pos.step");
        NodeId p1 = w.addProgram("WeldLeft",  r1);
        NodeId p2 = w.addProgram("WeldRight", r2);
        w.addOperation(p1, "Home",  "MoveJ");
        w.addOperation(p1, "Weld1", "MoveL");
        w.addOperation(p2, "Home",  "MoveJ");
        w.saveAs(tmp.path.string());
    }

    Project r(backend);
    REQUIRE(r.open(tmp.path.string()));

    REQUIRE(r.name()             == "MultiRobot");
    REQUIRE(r.robots().size()     == 2);
    REQUIRE(r.workpieces().size() == 1);
    REQUIRE(r.fixtures().size()   == 1);
    REQUIRE(r.programs().size()   == 2);
    REQUIRE(r.operations(r.programs()[0]).size() == 2);
    REQUIRE(r.operations(r.programs()[1]).size() == 1);

    REQUIRE(r.itemName(r.robots()[0]) == "UR10e");
    REQUIRE(r.itemName(r.robots()[1]) == "KR210");
    REQUIRE(r.itemName(r.programs()[0]) == "WeldLeft");
    REQUIRE(r.itemName(r.programs()[1]) == "WeldRight");
}
