// ------------------------------------------------------------------------------------
// Project: CellForge
// Copyright (C) 2026, Melina Aero Teknoloji Gelistirme ve Dizayn Burosu A.S., Istanbul
// Author: Onur Tuncer, PhD
//
// SPDX-License-Identifier: LGPL-2.1-only
// License-Filename: LICENSE
// ------------------------------------------------------------------------------------

#pragma once

#include <gp_Dir.hxx>
#include <gp_Trsf.hxx>
#include <flecs.h>
#include <string>
#include <vector>

namespace cellforge::workcell {

// ─── Identity ─────────────────────────────────────────────────────────────────

struct Name { std::string value; };

// ─── Transforms ───────────────────────────────────────────────────────────────

// Pose relative to the ChildOf parent (set by the joint/loader).
// gp_Trsf default-constructs to the identity transformation.
struct LocalTransform { gp_Trsf value; };

// World-space pose — written by TransformSystem each tick, read by everything else.
struct WorldTransform { gp_Trsf value; };

// ─── Kinematics ───────────────────────────────────────────────────────────────

enum class JointType { Revolute, Prismatic, Fixed, Continuous };

struct JointData {
    JointType type           = JointType::Revolute;
    gp_Dir    axis           = gp_Dir(0.0, 0.0, 1.0);  // default: Z-axis
    double    lower_limit    = 0.0;
    double    upper_limit    = 0.0;
    double    value          = 0.0;   // current position (rad or m)
    double    velocity_limit = 0.0;
    double    effort_limit   = 0.0;
};

// Attached to the RobotTag entity; describes the ordered joint chain.
struct KinematicChain {
    std::string                  base_link;
    std::string                  tip_link;
    std::vector<flecs::entity_t> joints;   // ordered base → tip
};

// ─── Geometry / collision (opaque handles — filled by cellforge_cad) ──────────

struct GeometryRef  { uint64_t id = 0; };
struct CollisionRef { uint64_t id = 0; };

// ─── Visual ───────────────────────────────────────────────────────────────────

struct Material {
    float r = 0.7f, g = 0.7f, b = 0.7f, a = 1.0f;
};

// ─── Scene state flags ────────────────────────────────────────────────────────

struct Visible  { bool value = true;  };
struct Frozen   { bool value = false; };
struct Selected { bool value = false; };

}  // namespace cellforge::workcell
