

//TODO platform should live outside core folder!

// platform/qt/QtEventMapper.h
#pragma once
#include <QKeyEvent>
#include <QMouseEvent>
#include <QWheelEvent>

#include "core/event/Event.h"

namespace CellForge {

class QtEventMapper {
public:
    static KeyCode mapKey(int qtKey);
    static MouseCode mapMouseButton(Qt::MouseButton button);
};

}