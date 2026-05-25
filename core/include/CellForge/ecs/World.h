

#pragma once

#include <flecs.h>

namespace CellForge {

class World {
public:
    World();

    flecs::world& flecsWorld();

private:
    flecs::world m_World;
};

}