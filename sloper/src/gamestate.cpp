#include "gamestate.h"
#include "helpers.h"

GameState::GameState(AnimatableImage *g,
                     AnimatableImage *t1,
                     AnimatableImage *t2,
                     AnimatableImage *t3,
                     QTimer *c,
                     QElapsedTimer *e,
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
    , elapsed(e)
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
    if (elapsed->elapsed() > bestScore) {
        bestScore = elapsed->elapsed();
    }
}
// Resets the game to intial state (doesn't clear clock)
void GameState::reset()
{
    paused = false;
    animation1 = Helpers().startAnimation(tree1, 50, -850, 8000);
    animation2 = Helpers().startAnimation(tree2, 200, -150, 5000);
    animation3 = Helpers().startAnimation(tree3, 350, -1200, 12000);
    elapsed->restart();
    colTimer->start(10);
}

int GameState::getBestScore()
{
    return bestScore;
}
