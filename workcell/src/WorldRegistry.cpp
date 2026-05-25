#include "WorldRegistry.h"
#include "Components.h"
#include "Tags.h"

namespace CellForge::workcell {

WorldRegistry::WorldRegistry() {
    register_components();
    register_systems();
}

void WorldRegistry::tick(float delta_seconds) {
    world_.progress(delta_seconds);
}

void WorldRegistry::register_components() {
    // Pre-registration gives each type a stable name in the flecs explorer.
    world_.component<Name>("Name");
    world_.component<LocalTransform>("LocalTransform");
    world_.component<WorldTransform>("WorldTransform");
    world_.component<JointData>("JointData");
    world_.component<KinematicChain>("KinematicChain");
    world_.component<GeometryRef>("GeometryRef");
    world_.component<CollisionRef>("CollisionRef");
    world_.component<Material>("Material");
    world_.component<Visible>("Visible");
    world_.component<Frozen>("Frozen");
    world_.component<Selected>("Selected");

    world_.component<RobotTag>("RobotTag");
    world_.component<LinkTag>("LinkTag");
    world_.component<JointTag>("JointTag");
    world_.component<ToolTag>("ToolTag");
    world_.component<FixtureTag>("FixtureTag");
    world_.component<WorkpieceTag>("WorkpieceTag");
    world_.component<FrameTag>("FrameTag");
}

void WorldRegistry::register_systems() {
    // Propagate LocalTransform up the ChildOf hierarchy to compute WorldTransform.
    // flecs schedules systems in topological ChildOf order, so a parent's
    // WorldTransform is always resolved before its children run.
    world_.system<const LocalTransform, WorldTransform>("TransformSystem")
        .kind(flecs::OnUpdate)
        .each([](flecs::entity e,
                 const LocalTransform& local,
                 WorldTransform&       world_tf)
        {
            flecs::entity parent = e.target(flecs::ChildOf);
            if (parent && parent.has<WorldTransform>()) {
                // gp_Trsf::Multiplied returns parent * local as a new transform.
                world_tf.value = parent.get<WorldTransform>()->value.Multiplied(local.value);
            } else {
                world_tf.value = local.value;
            }
        });
}

}  // namespace cellforge::workcell
