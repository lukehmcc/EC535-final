#include "gamestate.h"

GameState::GameState(AnimatableImage *g,
                     AnimatableImage *t1,
                     AnimatableImage *t2,
                     AnimatableImage *t3,
                     QTimer *c,
                     QPropertyAnimation *a1,
                     QPropertyAnimation *a2,
                     QPropertyAnimation *a3)
    : paused(false)
    , guy(g)
    , tree1(t1)
    , tree2(t2)
    , tree3(t3)
    , animation1(a1)
    , animation2(a2)
    , animation3(a3)
    , colTimer(c)
{}

bool GameState::isPaused()
{
    return paused;
}
// Function that stops the game
void GameState::stop()
{
    paused = true;
    animation1->stop();
    animation2->stop();
    animation3->stop();
    colTimer->stop();
}
// Resets the game to intial state (doesn't clear clock)
void GameState::reset()
{
    paused = false;
}
