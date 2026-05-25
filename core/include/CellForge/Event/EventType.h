

#pragma once

namespace CellForge{

    
enum class EventType {
    None,

    KeyPressed,
    KeyReleased,
    MouseButtonPressed,
    MouseButtonReleased,
    MouseMoved,
    MouseScrolled,

    WindowResize,
    WindowClose,

    AppTick,
    AppUpdate,
    AppRender
};

enum class EventCategory : uint32_t {
    None        = 0,
    Application = 1 << 0,
    Input       = 1 << 1,
    Keyboard    = 1 << 2,
    Mouse       = 1 << 3,
    MouseButton = 1 << 4
};
    

}
