// ------------------------------------------------------------------------------------
// Project: CellForge
// Copyright (C) 2026, Melina Aero Teknoloji Gelistirme ve Dizayn Burosu A.S., Istanbul
// Author: Onur Tuncer, PhD
//
// SPDX-License-Identifier: LGPL-2.1-only
// License-Filename: LICENSE
// ------------------------------------------------------------------------------------

#pragma once

#include <cstdint>

namespace CellForge {

// ─── Primitive geometry types ─────────────────────────────────────────────────
// These types are backend-independent and carry no OCCT dependency.
// They replace gp_Dir / gp_Trsf in headers that must remain OCCT-free.

struct Vec3 {
    double x = 0.0, y = 0.0, z = 0.0;
};

// Unit-direction vector.  Callers are responsible for normalisation.
struct Dir3 {
    double x = 0.0, y = 0.0, z = 1.0;
};

// Row-major homogeneous 4×4 transform (rigid-body by convention).
// data[row * 4 + col].  Default-constructs to the identity.
struct Transform {
    double data[16] = {
        1, 0, 0, 0,
        0, 1, 0, 0,
        0, 0, 1, 0,
        0, 0, 0, 1
    };

    static constexpr Transform identity() noexcept { return Transform{}; }

    // Read / write helpers (no bounds checking).
    double  operator()(int row, int col) const { return data[row * 4 + col]; }
    double& operator()(int row, int col)       { return data[row * 4 + col]; }
};

struct BoundingBox {
    Vec3 min;
    Vec3 max;
    bool empty = true;
};

}  // namespace CellForge
