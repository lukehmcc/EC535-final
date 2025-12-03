#include <QPropertyAnimation>
#include <QTimer>
#include "animatableimage.h"

#ifndef GAMESTATE_H
#define GAMESTATE_H

class GameState
{
public:
    GameState(AnimatableImage *g,
              AnimatableImage *t1,
              AnimatableImage *t2,
              AnimatableImage *t3,
              QTimer *c,
              QPropertyAnimation *a1,
              QPropertyAnimation *a2,
              QPropertyAnimation *a3);
    AnimatableImage *guy;
    AnimatableImage *tree1;
    AnimatableImage *tree2;
    AnimatableImage *tree3;
    QPropertyAnimation *animation1;
    QPropertyAnimation *animation2;
    QPropertyAnimation *animation3;
    QTimer *colTimer;
    bool isPaused();
    // Function that stops the game
    void stop();
    // Resets the game to intial state (doesn't clear clock)
    void reset();

private:
    bool paused;
};

#endif // GAMESTATE_H
