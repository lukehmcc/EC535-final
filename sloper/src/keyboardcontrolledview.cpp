#include "keyboardcontrolledview.h"

KeyboardControlledView::KeyboardControlledView(QGraphicsScene *scene, AnimatableImage *skier)
    : QGraphicsView(scene)
    , m_skier(skier)
{
    // enable keyboard focus
    setFocusPolicy(Qt::StrongFocus);
    // mvement step size (in pixels)
    m_stepSize = 150.0;
};

void KeyboardControlledView::keyPressEvent(QKeyEvent *event)
{
    // get current position
    QPointF currentPos = m_skier->pos();
    QPointF newPos = currentPos;

    // handle arrow key movement
    switch (event->key()) {
    case Qt::Key_Up:
        if (is_bottom) {
            is_bottom = false;
            newPos.setY(currentPos.y() - m_stepSize);
        } else if (!is_top) {
            is_top = true;
            newPos.setY(currentPos.y() - m_stepSize);
        }
        break;
    case Qt::Key_Down:
        if (is_top) {
            is_top = false;
            newPos.setY(currentPos.y() + m_stepSize);
        } else if (!is_bottom) {
            is_bottom = true;
            newPos.setY(currentPos.y() + m_stepSize);
        }
        break;
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
    default:
        QGraphicsView::keyPressEvent(event);
        return;
    }

    // update skier position
    m_skier->setPos(newPos);
}
