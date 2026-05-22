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
