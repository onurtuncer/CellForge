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
#include <memory>
#include <filesystem>

#include <Eigen/Core>
#include <pinocchio/multibody/model.hpp>
#include <pinocchio/multibody/data.hpp>
#include <tesseract_environment/environment.h>
#include <tesseract_common/resource_locator.h>

#include "cellforge/robot/RoboState.hpp"

namespace cellforge {

/// Loaded robot: owns both the Tesseract environment and the Pinocchio model.
/// Single source of truth for joint ordering and limits.

class RobotModel {
public:
    RobotModel() = default;

    // ── Loading ────────────────────────────────────────────────────────────

    struct LoadResult {
        bool        ok      = false;
        std::string error;          // non-empty on failure
    };

    /// Load from file paths. mesh_search_dir is the root for package:// URIs.
    LoadResult load(const std::filesystem::path& urdf_path,
                    const std::filesystem::path& srdf_path,
                    const std::filesystem::path& mesh_search_dir = {});

    /// Load from in-memory strings (useful for tests or string-based config).
    LoadResult loadFromStrings(const std::string& urdf_xml,
                               const std::string& srdf_xml,
                               const std::filesystem::path& mesh_search_dir = {});

    bool isLoaded() const { return loaded_; }

    // ── Joint info ─────────────────────────────────────────────────────────

    /// Canonical ordered joint names (actuated joints only, same order in both backends).
    const std::vector<std::string>& jointNames() const { return joint_names_; }
    int                             dof()         const { return static_cast<int>(joint_names_.size()); }

    struct JointLimits { double lower, upper; };
    JointLimits limits(std::string_view name) const;

    // ── State ──────────────────────────────────────────────────────────────

    /// Zero / neutral state with all joint names populated.
    RoboState makeZeroState() const;

    /// Push a state to BOTH backends atomically.
    /// After this call: Tesseract env has updated link transforms,
    /// Pinocchio data_ has updated FK.
    void setState(const RobotState& state);

    /// Read current state back from Tesseract (useful after IK writes to env).
    RoboState getState() const;

    // ── Backend access ─────────────────────────────────────────────────────

    tesseract_environment::Environment& tesseractEnv()        { return *env_; }
    const tesseract_environment::Environment& tesseractEnv()  const { return *env_; }

    pinocchio::Model&       pinocchioModel()       { return pin_model_; }
    const pinocchio::Model& pinocchioModel() const { return pin_model_; }

    pinocchio::Data&        pinocchioData()        { return pin_data_; }
    const pinocchio::Data&  pinocchioData()  const { return pin_data_; }

private:
    bool                    loaded_ = false;
    std::vector<std::string> joint_names_;   // canonical order

    // Tesseract
    std::shared_ptr<tesseract_environment::Environment>    env_;
    std::shared_ptr<tesseract_common::ResourceLocator>     locator_;

    // Pinocchio
    pinocchio::Model  pin_model_;
    pinocchio::Data   pin_data_;

    // Internal helpers
    LoadResult initTesseract(const std::filesystem::path& urdf,
                             const std::filesystem::path& srdf);
    LoadResult initPinocchio(const std::filesystem::path& urdf);
    void       buildJointIndex();   // fills joint_names_ from intersection
    void       applyStateToBothBackends(const RoboState& s);
};

} // namespace cellforge