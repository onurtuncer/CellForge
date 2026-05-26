// ------------------------------------------------------------------------------------
// Project: CellForge
// Copyright (C) 2026, Melina Aero Teknoloji Gelistirme ve Dizayn Burosu A.S., Istanbul
// Author: Onur Tuncer, PhD
//
// SPDX-License-Identifier: LGPL-2.1-only
// License-Filename: LICENSE
// ------------------------------------------------------------------------------------

#pragma once

namespace cellforge::workcell {

struct RobotTag     {};   // manipulator root
struct LinkTag      {};   // rigid body in a kinematic chain
struct JointTag     {};   // articulation between two links
struct ToolTag      {};   // end-effector (attached to a flange)
struct FixtureTag   {};   // static scene object (jig, clamp, table)
struct WorkpieceTag {};   // part being processed
struct FrameTag     {};   // named coordinate frame (user frame, TCP override)

}  // namespace cellforge::workcell
