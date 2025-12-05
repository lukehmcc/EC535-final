#include <QGraphicsView>
#include <QKeyEvent>

#include "animatableimage.h"
#include "gamestate.h"

#ifndef SONARCONTROLLEDVIEW_H
#define SONARCONTROLLEDVIEW_H

// class to handle sonar input
class SonarControlledView : public QGraphicsView
{
    Q_OBJECT

public:
    SonarControlledView(QGraphicsScene *scene, GameState *state);

private:
    GameState *state;
    qreal m_stepSize;
    bool is_left = false;
    bool is_right = false;
    bool is_top = false;
    bool is_bottom = false;
};

#endif // SONARCONTROLLEDVIEW_H
