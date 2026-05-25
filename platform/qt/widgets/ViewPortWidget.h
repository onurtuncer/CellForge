

#pragma once

#include <QWidget>

#include <memory>

#include "core/event/EventQueue.h"
#include "platform/qt/QtEventMapper.h"

class QKeyEvent;
class QMouseEvent;
class QWheelEvent;
class QResizeEvent;
class QCloseEvent;

namespace CellForge {

class ViewportWidget : public QWidget {
    Q_OBJECT

public:
    explicit ViewportWidget(QWidget* parent = nullptr);
    ~ViewportWidget() override = default;

    EventQueue& eventQueue();
    const EventQueue& eventQueue() const;

protected:
    void keyPressEvent(QKeyEvent* event) override;
    void keyReleaseEvent(QKeyEvent* event) override;

    void mousePressEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void wheelEvent(QWheelEvent* event) override;

    void resizeEvent(QResizeEvent* event) override;
    void closeEvent(QCloseEvent* event) override;

private:
    EventQueue m_EventQueue;
};

} // namespace CellForge