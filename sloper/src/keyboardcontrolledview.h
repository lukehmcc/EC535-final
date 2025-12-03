#include <QGraphicsView>
#include <QKeyEvent>
#include "animatableimage.h"

#ifndef KEYBOARDCONTROLLEDVIEW_H
#define KEYBOARDCONTROLLEDVIEW_H

// class to handle keyboard input
class KeyboardControlledView : public QGraphicsView
{
    Q_OBJECT

public:
    KeyboardControlledView(QGraphicsScene *scene, AnimatableImage *skier);

protected:
    void keyPressEvent(QKeyEvent *event) override;

private:
    AnimatableImage *m_skier;
    qreal m_stepSize;
    bool is_left = false;
    bool is_right = false;
    bool is_top = false;
    bool is_bottom = false;
};

#endif // KEYBOARDCONTROLLEDVIEW_H
