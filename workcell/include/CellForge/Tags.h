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
