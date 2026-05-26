// ------------------------------------------------------------------------------------
// Project: CellForge
// Copyright (C) 2026, Melina Aero Teknoloji Gelistirme ve Dizayn Burosu A.S., Istanbul
// Author: Onur Tuncer, PhD
//
// SPDX-License-Identifier: LGPL-2.1-only
// License-Filename: LICENSE
// ------------------------------------------------------------------------------------

#pragma once

#include <flecs.h>
#include <string>

namespace cellforge::workcell {

class WorldRegistry;

// Creates workcell entities from robot description files.
class SceneLoader {
public:
    explicit SceneLoader(WorldRegistry& registry);

    // Parse a URDF (and optionally an SRDF) and populate the world with
    // RobotTag / LinkTag / JointTag entities connected via flecs ChildOf.
    // Returns the root robot entity.
    flecs::entity load_urdf(const std::string& urdf_path,
                            const std::string& srdf_path = {});

private:
    WorldRegistry& registry_;
};

}  // namespace cellforge::workcell
