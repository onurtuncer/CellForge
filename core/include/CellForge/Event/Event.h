

// core/event/Event.h
#pragma once

namespace CellForge {



class Event {
public:
    virtual ~Event() = default;

    virtual EventType type() const = 0;
    virtual uint32_t categoryFlags() const = 0;

    bool handled = false;

    bool isInCategory(EventCategory category) const {
        return categoryFlags() & static_cast<uint32_t>(category);
    }
};

}