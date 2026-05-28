// ------------------------------------------------------------------------------------
// Project: CellForge
// Copyright (C) 2026, Melina Aero Teknoloji Gelistirme ve Dizayn Burosu A.S., Istanbul
// Author: Onur Tuncer, PhD
//
// SPDX-License-Identifier: LGPL-2.1-only
// License-Filename: LICENSE
// ------------------------------------------------------------------------------------

#pragma once

#include "CellForge/Mathematics.h"

#include <cstdint>
#include <string_view>
#include <vector>

namespace CellForge {

// ─── Opaque shape handle ──────────────────────────────────────────────────────
// The backend owns the lifetime of every shape it creates.
// Handles are stable for the backend's lifetime; copying a handle does NOT
// copy the shape.  0 is the null / invalid sentinel.
using ShapeId = uint64_t;
inline constexpr ShapeId NullShape = 0;

// ─── IGeometryBackend ────────────────────────────────────────────────────────
// Pure interface that isolates all B-rep / CAD operations behind a single
// boundary.  Concrete implementations (e.g. OcctGeometryBackend) live in
// their own library and are the only translation units that #include OCCT.
//
// Ownership rules
//   • Every factory method (make*, load*, fuse, cut, common)
//     returns a new ShapeId owned by the backend.
//   • The caller must call release() exactly once per returned handle when the
//     shape is no longer needed.
//   • Boolean-op inputs are NOT released by the operation; the caller retains
//     ownership of a and b.
//   • Returning NullShape signals failure; no handle to release.

class IGeometryBackend {
public:
    virtual ~IGeometryBackend() = default;

    // ── Primitives ────────────────────────────────────────────────────────────
    // Shapes are created at the origin; use applyTransform to position them.

    virtual ShapeId makeBox     (double dx, double dy, double dz)         = 0;
    virtual ShapeId makeCylinder(double radius, double height)            = 0;
    virtual ShapeId makeSphere  (double radius)                           = 0;
    // r1 / r2: bottom / top radii.  Set r2=0 for a true cone.
    virtual ShapeId makeCone    (double r1, double r2, double height)     = 0;

    // ── I/O ───────────────────────────────────────────────────────────────────

    virtual ShapeId loadStep(std::string_view path) = 0;
    virtual ShapeId loadIges(std::string_view path) = 0;
    virtual ShapeId loadBrep(std::string_view path) = 0;

    virtual bool saveStep(ShapeId h, std::string_view path) = 0;
    virtual bool saveBrep(ShapeId h, std::string_view path) = 0;

    // ── Boolean operations ────────────────────────────────────────────────────

    virtual ShapeId fuse  (ShapeId a, ShapeId b) = 0;   // union
    virtual ShapeId cut   (ShapeId a, ShapeId b) = 0;   // a minus b
    virtual ShapeId common(ShapeId a, ShapeId b) = 0;   // intersection

    // ── Transform ─────────────────────────────────────────────────────────────

    virtual void applyTransform(ShapeId h, const Transform& t) = 0;

    // ── Tessellation ──────────────────────────────────────────────────────────
    // Converts a B-rep shape to a triangle mesh suitable for rendering.
    // linearDeflection  — max chord error in model units (e.g. 0.1 mm).
    // angularDeflection — max angular error in radians (e.g. 0.5).

    struct MeshData {
        std::vector<float>    positions;  // 3 floats per vertex  [x,y,z, ...]
        std::vector<float>    normals;    // 3 floats per vertex  (may be empty)
        std::vector<uint32_t> indices;    // triangle list: 3 indices per triangle
    };

    virtual MeshData tessellate(ShapeId h,
                                double linearDeflection,
                                double angularDeflection) = 0;

    // ── Properties ────────────────────────────────────────────────────────────

    virtual BoundingBox boundingBox(ShapeId h) = 0;
    virtual double      volume     (ShapeId h) = 0;
    virtual double      surfaceArea(ShapeId h) = 0;
    virtual bool        isValid    (ShapeId h) = 0;

    // ── Lifecycle ─────────────────────────────────────────────────────────────

    virtual void release(ShapeId h) = 0;
};

}  // namespace CellForge
