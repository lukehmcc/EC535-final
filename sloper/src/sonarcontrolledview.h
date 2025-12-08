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
    SonarControlledView(GameState *state);
    ~SonarControlledView();

protected:
    void readSonarData();

private:
    GameState *state;
    QTimer *sonarClock;
    qreal m_stepSize;
    QFile file;
    int pos_left;
    int pos_center;
    int pos_right;
    bool is_left = false;
    bool is_right = false;
    bool is_top = false;
    bool is_bottom = false;
};

#endif // SONARCONTROLLEDVIEW_H
