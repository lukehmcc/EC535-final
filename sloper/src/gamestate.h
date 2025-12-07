#include <QElapsedTimer>
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
              AnimatableImage *t4,
              AnimatableImage *t5,
              AnimatableImage *t6,
              QTimer *c,
              QElapsedTimer *e);
    AnimatableImage *guy;
    AnimatableImage *tree1;
    AnimatableImage *tree2;
    AnimatableImage *tree3;
    AnimatableImage *tree4;
    AnimatableImage *tree5;
    AnimatableImage *tree6;
    QPropertyAnimation *animation1;
    QPropertyAnimation *animation2;
    QPropertyAnimation *animation3;
    QPropertyAnimation *animation4;
    QPropertyAnimation *animation5;
    QPropertyAnimation *animation6;
    QTimer *colTimer;
    QElapsedTimer *elapsed;
    bool isPaused();
    // Function that stops the game
    void stop();
    // Resets the game to intial state (doesn't clear clock)
    void reset();
    int getBestScore();
    void putTreesOffScreen();
    bool paused;
    bool titleScreen;

private:
    int bestScore;
    int animationTime;
    QTimer *animationTimer;
    void kickStartTrees();
    bool isFirstBatch;
};

#endif // GAMESTATE_H
