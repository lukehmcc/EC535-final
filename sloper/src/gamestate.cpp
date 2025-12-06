#include "gamestate.h"
#include <QDebug>
#include <QThread>
#include <QTimer>
#include "helpers.h"
#include <iostream>

GameState::GameState(AnimatableImage *g,
                     AnimatableImage *t1,
                     AnimatableImage *t2,
                     AnimatableImage *t3,
                     QTimer *c,
                     QElapsedTimer *e)
    : paused(false)
    , guy(g)
    , tree1(t1)
    , tree2(t2)
    , tree3(t3)
    , colTimer(c)
    , elapsed(e)
{
    bestScore = 0;         // init
    animationTime = 10000; // 6s
    kickStartTrees();
}

bool GameState::isPaused()
{
    return paused;
}
// Function that stops the game
void GameState::stop()
{
    paused = true;
    if (animation1) {
        animation1->stop();
    }
    if (animation2) {
        animation2->stop();
    }
    if (animation3) {
        animation3->stop();
    }
    animationTimer->stop();
    colTimer->stop();
    if (elapsed->elapsed() > bestScore) {
        bestScore = elapsed->elapsed();
    }
}
// Resets the game to intial state (doesn't clear clock)
void GameState::reset()
{
    paused = false;
    animationTime = 10000; // 6s
    animationTimer->start(animationTime);
    elapsed->restart();
    kickStartTrees();
    colTimer->start(10);
}

int GameState::getBestScore()
{
    return bestScore;
}

// gives a proportional time based on offset
int proportionalTime(double distance1, int distance2, int time)
{
    return (static_cast<double>(distance2 - 1150) / (distance1 - 1150)) * time;
}

// restarts the tree animations
void GameState::kickStartTrees()
{
    int t1 = proportionalTime(-1200, -850, animationTime);
    int t2 = proportionalTime(-1200, -150, animationTime);
    int t3 = proportionalTime(-1200, -1200, animationTime);
    animation1 = Helpers().startAnimation(tree1, 50, -850, t1);
    animation2 = Helpers().startAnimation(tree2, 200, -150, t2);
    animation3 = Helpers().startAnimation(tree3, 350, -1200, t3);
    // tree controller logic
    // infinite while loop
    animationTimer = new QTimer();
    QObject::connect(animationTimer, &QTimer::timeout, [this]() {
        int t1 = proportionalTime(-1200, -850, animationTime);
        int t2 = proportionalTime(-1200, 0, animationTime);
        int t3 = proportionalTime(-1200, -1200, animationTime);
        animation1 = Helpers().startAnimation(tree1, 50, -850, t1);
        animation2 = Helpers().startAnimation(tree2, 200, 0, t2);
        animation3 = Helpers().startAnimation(tree3, 350, -1200, t3);
        animationTime = std::max(animationTime - 100, 100);
        animationTimer->setInterval(animationTime);
    });
    animationTimer->start(animationTime);
}
