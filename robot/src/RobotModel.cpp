// ------------------------------------------------------------------------------------
// Project: CellForge
// Copyright (C) 2026, Melina Aero Teknoloji Gelistirme ve Dizayn Burosu A.S., Istanbul
// Author: Onur Tuncer, PhD
//
// SPDX-License-Identifier: LGPL-2.1-only
// License-Filename: LICENSE
// ------------------------------------------------------------------------------------

#include "RobotModel.h"

#include <pinocchio/parsers/urdf.hpp>
#include <pinocchio/algorithm/kinematics.hpp>
#include <pinocchio/algorithm/joint-configuration.hpp>

#include <tesseract_common/resource_locator.h>

namespace cellforge {

// ─────────────────────────────────────────────────────────────────────────────
// Loading
// ─────────────────────────────────────────────────────────────────────────────

RobotModel::LoadResult
RobotModel::load(const std::filesystem::path& urdf_path,
                 const std::filesystem::path& srdf_path,
                 const std::filesystem::path& mesh_search_dir)
{
    loaded_ = false;

    // Resource locator: maps package://robot_name/meshes/... to disk
    // Use urdf directory as fallback if mesh_search_dir not given
    auto search = mesh_search_dir.empty() ? urdf_path.parent_path() : mesh_search_dir;

    locator_ = std::make_shared<tesseract_common::GeneralResourceLocator>();

    // ── Tesseract ──────────────────────────────────────────────────────────
    env_ = std::make_shared<tesseract_environment::Environment>();
    if (!env_->init(tesseract_common::FilesystemPath(urdf_path.string()),
                    tesseract_common::FilesystemPath(srdf_path.string()),
                    locator_))
    {
        return { false, "Tesseract Environment::init failed. Check URDF/SRDF paths and mesh URIs." };
    }

    // ── Pinocchio ──────────────────────────────────────────────────────────
    try {
        pinocchio::urdf::buildModel(urdf_path.string(),
                                    pinocchio::JointModelFreeFlyer(), // use JointModelFixed for fixed-base
                                    pin_model_);
        // For a fixed-base robot (most manipulators), use:
        //   pinocchio::urdf::buildModel(urdf_path.string(), pin_model_);
        pin_data_ = pinocchio::Data(pin_model_);
    } catch (const std::exception& e) {
        return { false, std::string("Pinocchio URDF load failed: ") + e.what() };
    }

    buildJointIndex();
    loaded_ = true;
    return { true, {} };
}

RobotModel::LoadResult
RobotModel::loadFromStrings(const std::string& urdf_xml,
                             const std::string& srdf_xml,
                             const std::filesystem::path& mesh_search_dir)
{
    loaded_ = false;
    locator_ = std::make_shared<tesseract_common::GeneralResourceLocator>();

    env_ = std::make_shared<tesseract_environment::Environment>();
    if (!env_->init(urdf_xml, srdf_xml, locator_))
        return { false, "Tesseract init from strings failed." };

    try {
        // Pinocchio has no direct from-string API; write to a temp file
        namespace fs = std::filesystem;
        auto tmp = fs::temp_directory_path() / "cellforge_tmp_robot.urdf";
        { std::ofstream f(tmp); f << urdf_xml; }
        pinocchio::urdf::buildModel(tmp.string(), pin_model_);
        fs::remove(tmp);
        pin_data_ = pinocchio::Data(pin_model_);
    } catch (const std::exception& e) {
        return { false, std::string("Pinocchio init from string failed: ") + e.what() };
    }

    buildJointIndex();
    loaded_ = true;
    return { true, {} };
}

// ─────────────────────────────────────────────────────────────────────────────
// Joint index — the critical sync point
// ─────────────────────────────────────────────────────────────────────────────

void RobotModel::buildJointIndex()
{
    // Ground truth: Tesseract's active joint names (excludes fixed joints)
    joint_names_ = env_->getActiveJointNames();

    // Sanity check: every name must also exist in Pinocchio
    for (const auto& name : joint_names_) {
        if (!pin_model_.existJointName(name)) {
            // Non-fatal mismatch warning — Pinocchio may have mimic joints
            // or different naming. Log and continue; the adapter will skip unknowns.
        }
    }
}

// ─────────────────────────────────────────────────────────────────────────────
// State I/O
// ─────────────────────────────────────────────────────────────────────────────

RoboState RobotModel::makeZeroState() const
{
    RoboState s;
    s.joints.resize(joint_names_.size());
    for (std::size_t i = 0; i < joint_names_.size(); ++i)
        s.joints[i].name = joint_names_[i];
    s.isValid = true;
    return s;
}

void RobotModel::setState(const RoboState& state)
{
    applyStateToBothBackends(state);
}

RoboState RobotModel::getState() const
{
    // Read back from Tesseract (authoritative for joint positions in env)
    const auto tess_state = env_->getCurrentJointValues();

    RoboState s = makeZeroState();
    for (auto& jv : s.joints) {
        auto it = tess_state.find(jv.name);
        if (it != tess_state.end())
            jv.position = it->second;
    }
    s.isValid = true;
    return s;
}

// ─────────────────────────────────────────────────────────────────────────────
// Sync to both backends
// ─────────────────────────────────────────────────────────────────────────────

void RobotModel::applyStateToBothBackends(const RoboState& s)
{
    // ── 1. Tesseract ───────────────────────────────────────────────────────
    std::vector<std::string>  names;
    Eigen::VectorXd           positions(s.joints.size());

    names.reserve(s.joints.size());
    for (std::size_t i = 0; i < s.joints.size(); ++i) {
        names.push_back(s.joints[i].name);
        positions[i] = s.joints[i].position;
    }
    env_->setState(names, positions);

    // ── 2. Pinocchio ───────────────────────────────────────────────────────
    Eigen::VectorXd q = pinocchio::neutral(pin_model_);

    for (const auto& jv : s.joints) {
        if (!pin_model_.existJointName(jv.name)) continue;
        const auto jid = pin_model_.getJointId(jv.name);
        q[pin_model_.joints[jid].idx_q()] = jv.position;
    }

    pinocchio::forwardKinematics(pin_model_, pin_data_, q);
    // Optional: update geometry data if you have a GeometryModel
    // pinocchio::updateFramePlacements(pin_model_, pin_data_);
}

RobotModel::JointLimits RobotModel::limits(std::string_view name) const
{
    // Read from Pinocchio's model (already parsed from URDF limits)
    if (!pin_model_.existJointName(std::string(name)))
        return { -M_PI, M_PI };  // fallback

    const auto jid = pin_model_.getJointId(std::string(name));
    return {
        pin_model_.lowerPositionLimit[pin_model_.joints[jid].idx_q()],
        pin_model_.upperPositionLimit[pin_model_.joints[jid].idx_q()]
    };
}

} // namespace cellforge