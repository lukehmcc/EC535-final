#include "keyboardcontrolledview.h"

KeyboardControlledView::KeyboardControlledView(QGraphicsScene *scene, GameState *state)
    : QGraphicsView(scene)
    , state(state)
{
    // enable keyboard focus
    setFocusPolicy(Qt::StrongFocus);
    // mvement step size (in pixels)
    m_stepSize = 70.0;
};

void KeyboardControlledView::keyPressEvent(QKeyEvent *event)
{
    // get current position
    QPointF currentPos = state->guy->pos();
    QPointF newPos = currentPos;

    // if any key event happens you wanna restart (if it's stopped)
    if (state->isPaused()) {
        state->reset();
    }

    // handle arrow key movement
    switch (event->key()) {
    case Qt::Key_Left:
        if (is_right) {
            is_right = false;
            newPos.setX(currentPos.x() - m_stepSize);
        } else if (!is_left) {
            is_left = true;
            newPos.setX(currentPos.x() - m_stepSize);
        }
        break;
    case Qt::Key_Right:
        if (is_left) {
            is_left = false;
            newPos.setX(currentPos.x() + m_stepSize);
        } else if (!is_right) {
            is_right = true;
            newPos.setX(currentPos.x() + m_stepSize);
        }
        break;
    case Qt::Key_Escape:
        state->titleScreen = true;
    default:
        QGraphicsView::keyPressEvent(event);
        return;
    }

    // update skier position
    state->guy->setPos(newPos);
}

void KeyboardControlledView::mousePressEvent(QMouseEvent *event)
{
    // if any mouse/touch event happens you wanna restart (if it's stopped)
    if (state->isPaused()) {
        state->reset();
    }
    
    // Call base class implementation
    QGraphicsView::mousePressEvent(event);
}
