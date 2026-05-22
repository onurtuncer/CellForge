#include "SceneLoader.h"
#include "Components.h"
#include "Tags.h"
#include "WorldRegistry.h"

namespace cellforge::workcell {

SceneLoader::SceneLoader(WorldRegistry& registry)
    : registry_(registry) {}

flecs::entity SceneLoader::load_urdf(const std::string& urdf_path,
                                     const std::string& /*srdf_path*/)
{
    // TODO: parse URDF with urdfdom, create LinkTag/JointTag entities
    //       connected via flecs ChildOf, populate JointData and GeometryRef.
    flecs::entity robot = registry_.world().entity()
        .add<RobotTag>()
        .set<Name>({ urdf_path })
        .set<LocalTransform>({})
        .set<WorldTransform>({});
    return robot;
}

}  // namespace cellforge::workcell
