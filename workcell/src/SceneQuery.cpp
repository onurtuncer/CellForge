// ------------------------------------------------------------------------------------
// Project: CellForge
// Copyright (C) 2026, Melina Aero Teknoloji Gelistirme ve Dizayn Burosu A.S., Istanbul
// Author: Onur Tuncer, PhD
//
// SPDX-License-Identifier: LGPL-2.1-only
// License-Filename: LICENSE
// ------------------------------------------------------------------------------------

#include "SceneQuery.h"
#include "Components.h"
#include "Tags.h"
#include "WorldRegistry.h"

namespace cellforge::workcell {

SceneQuery::SceneQuery(WorldRegistry& registry)
    : registry_(registry) {}

std::optional<flecs::entity> SceneQuery::find_by_name(const std::string& name) const {
    flecs::entity result;
    registry_.world().each<const Name>([&](flecs::entity e, const Name& n) {
        if (!result && n.value == name)
            result = e;
    });
    if (result)
        return result;
    return std::nullopt;
}

std::optional<gp_Trsf> SceneQuery::tcp_pose(flecs::entity robot) const {
    if (!robot.has<KinematicChain>())
        return std::nullopt;

    const auto* chain = robot.get<KinematicChain>();
    auto tip = find_by_name(chain->tip_link);
    if (!tip || !tip->has<WorldTransform>())
        return std::nullopt;

    return tip->get<WorldTransform>()->value;
}

std::vector<double> SceneQuery::joint_values(flecs::entity robot) const {
    if (!robot.has<KinematicChain>())
        return {};

    const auto* chain = robot.get<KinematicChain>();
    std::vector<double> values;
    values.reserve(chain->joints.size());
    for (flecs::entity_t jid : chain->joints) {
        flecs::entity je = registry_.world().entity(jid);
        if (je.has<JointData>())
            values.push_back(je.get<JointData>()->value);
    }
    return values;
}

}  // namespace cellforge::workcell
