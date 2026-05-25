

#pragma once

#include <cstdint>

namespace CellForge {

class World;

class Entity {
public:
    Entity() = default;

    Entity(uint64_t id, World* world);

    uint64_t id() const;

    bool valid() const;

private:
    uint64_t m_Id = 0;
    World* m_World = nullptr;
};

}