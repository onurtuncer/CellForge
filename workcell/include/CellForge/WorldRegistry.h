#pragma once

#include <flecs.h>

namespace cellforge::workcell {

// Owns the flecs world, pre-registers all components/tags, and installs built-in
// systems (TransformSystem, etc.).  One instance per application.
class WorldRegistry {
public:
    WorldRegistry();

    flecs::world&       world()       { return world_; }
    const flecs::world& world() const { return world_; }

    // Advance all registered systems by delta_seconds.
    void tick(float delta_seconds = 0.0f);

private:
    void register_components();
    void register_systems();

    flecs::world world_;
};

}  // namespace cellforge::workcell
