#pragma once

#include <gp_Trsf.hxx>
#include <flecs.h>
#include <optional>
#include <string>
#include <vector>

namespace cellforge::workcell {

class WorldRegistry;

// Convenience read-only queries over the ECS world.
class SceneQuery {
public:
    explicit SceneQuery(WorldRegistry& registry);

    // Find the first entity whose Name component equals `name`.
    std::optional<flecs::entity> find_by_name(const std::string& name) const;

    // World-space pose of the tip frame of `robot`'s KinematicChain.
    std::optional<gp_Trsf> tcp_pose(flecs::entity robot) const;

    // Ordered joint values (rad / m) for every joint in `robot`'s KinematicChain.
    std::vector<double> joint_values(flecs::entity robot) const;

private:
    WorldRegistry& registry_;
};

}  // namespace cellforge::workcell
