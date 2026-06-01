// ------------------------------------------------------------------------------------
// Project: CellForge
// Copyright (C) 2026, Melina Aero Teknoloji Gelistirme ve Dizayn Burosu A.S., Istanbul
// Author: Onur Tuncer, PhD
//
// SPDX-License-Identifier: LGPL-2.1-only
// License-Filename: LICENSE
// ------------------------------------------------------------------------------------

#pragma once

#include <string>
#include <vector>

namespace cellforge {

struct JointValue {
    std::string name;
    double      position = 0.0;
    double      velocity = 0.0;
};

/// Snapshot of the robot's full joint state.
struct RoboState {
    std::vector<JointValue> joints;
    bool                    isValid = false;
};

} // namespace cellforge
