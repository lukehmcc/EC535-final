#include <QGraphicsView>
#include <QKeyEvent>

#include "animatableimage.h"
#include "gamestate.h"

#ifndef KEYBOARDCONTROLLEDVIEW_H
#define KEYBOARDCONTROLLEDVIEW_H

// class to handle keyboard input
class KeyboardControlledView : public QGraphicsView
{
    Q_OBJECT

public:
    KeyboardControlledView(QGraphicsScene *scene, GameState *state);

protected:
    void keyPressEvent(QKeyEvent *event) override;

private:
    GameState *state;
    qreal m_stepSize;
    bool is_left = false;
    bool is_right = false;
    bool is_top = false;
    bool is_bottom = false;
};

#endif // KEYBOARDCONTROLLEDVIEW_H
