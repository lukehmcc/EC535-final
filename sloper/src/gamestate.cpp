#include "gamestate.h"
#include <QDebug>
#include <QThread>
#include <QTimer>
#include "helpers.h"
#include <random>

GameState::GameState(AnimatableImage *g,
                     AnimatableImage *t1,
                     AnimatableImage *t2,
                     AnimatableImage *t3,
                     AnimatableImage *t4,
                     AnimatableImage *t5,
                     AnimatableImage *t6,
                     QTimer *c,
                     QElapsedTimer *e)
    : paused(false)
    , guy(g)
    , tree1(t1)
    , tree2(t2)
    , tree3(t3)
    , tree4(t4)
    , tree5(t5)
    , tree6(t6)
    , colTimer(c)
    , elapsed(e)
{
    // null
    animation1 = nullptr;
    animation2 = nullptr;
    animation3 = nullptr;
    animation4 = nullptr;
    animation5 = nullptr;
    animation6 = nullptr;
    animationTimer = nullptr;

    // normal init
    bestScore = 0; // init
    paused = true;
    titleScreen = true;
}

bool GameState::isPaused()
{
    return paused;
}
// Function that stops the game
void GameState::stop()
{
    paused = true;
    if (animation1)
        animation1->stop();
    if (animation2)
        animation2->stop();
    if (animation3)
        animation3->stop();
    if (animation4)
        animation4->stop();
    if (animation5)
        animation5->stop();
    if (animation6)
        animation6->stop();
    if (animationTimer)
        animationTimer->stop();
    if (colTimer)
        colTimer->stop();
    if (elapsed && elapsed->elapsed() > bestScore) {
        bestScore = elapsed->elapsed();
    }
}
// Resets the game to intial state (doesn't clear clock)
void GameState::reset()
{
    putTreesOffScreen();
    paused = false;
    titleScreen = false;
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
    return (static_cast<double>(distance2 - 600) / (distance1 - 600)) * time;
}

static const std::array<std::array<int, 3>, 50> combos{
    {{-1150, -850, -450}, {-1100, -900, -500}, {-1050, -750, -350}, {-1000, -650, -250},
     {-950, -550, -150},  {-900, -700, -300},  {-850, -500, -200},  {-800, -600, -400},
     {-750, -450, -150},  {-700, -700, -350},  {-650, -750, -250},  {-600, -500, -300},
     {-550, -850, -400},  {-500, -650, -200},  {-450, -750, -350},  {-400, -600, -150},
     {-350, -700, -250},  {-150, -550, -450},  {-250, -650, -350},  {-200, -750, -400},
     {-1150, -650, -250}, {-1100, -700, -300}, {-1050, -600, -350}, {-1000, -750, -200},
     {-950, -500, -400},  {-900, -550, -450},  {-850, -600, -150},  {-800, -650, -350},
     {-750, -700, -250},  {-700, -450, -300},  {-650, -500, -400},  {-600, -850, -200},
     {-550, -750, -350},  {-500, -650, -250},  {-450, -550, -150},  {-400, -700, -400},
     {-350, -600, -300},  {-300, -750, -200},  {-250, -850, -350},  {-200, -500, -450},
     {-1150, -750, -350}, {-1100, -550, -250}, {-1050, -650, -200}, {-1000, -850, -400},
     {-950, -600, -300},  {-900, -750, -150},  {-850, -700, -400},  {-800, -500, -350},
     {-750, -550, -200},  {-700, -850, -300}}};

static std::mt19937 rng(std::random_device{}());

// restarts the tree animations
void GameState::kickStartTrees()
{
    // 1. Clean up existing timer if this function is called multiple times
    if (animationTimer) {
        animationTimer->stop();
        animationTimer->deleteLater();
    }

    animationTime = 10000; // 10s
    animationTimer = new QTimer();

    // checks if it is in group A or B of tree fires
    isFirstBatch = true;

    QObject::connect(animationTimer, &QTimer::timeout, [this]() {
        auto [p1, p2, p3] = combos[rng() % combos.size()];

        // Calculate durations based on current speed
        int t1 = proportionalTime(-1200, p1, animationTime);
        int t2 = proportionalTime(-1200, p2, animationTime);
        int t3 = proportionalTime(-1200, p3, animationTime);

        if (isFirstBatch) {
            qInfo() << "Tree set A: " << p1 << " " << p2 << " " << p3;
            animation1 = Helpers().startAnimation(tree1, 0, p1, t1);
            animation2 = Helpers().startAnimation(tree2, 100, p2, t2);
            animation3 = Helpers().startAnimation(tree3, 180, p3, t3);
        } else {
            qInfo() << "Tree set B: " << p1 << " " << p2 << " " << p3;
            animation4 = Helpers().startAnimation(tree4, 0, p1, t1);
            animation5 = Helpers().startAnimation(tree5, 100, p2, t2);
            animation6 = Helpers().startAnimation(tree6, 180, p3, t3);

            // only speed up on full cycles
            animationTime = std::max(animationTime - 250, 100);
        }
        isFirstBatch = !isFirstBatch;
        animationTimer->setInterval(animationTime / 2);
    });
    //	Fire first batch first
    {
        auto [p1, p2, p3] = combos[rng() % combos.size()];
        int t1 = proportionalTime(-1200, p1, animationTime);
        int t2 = proportionalTime(-1200, p2, animationTime);
        int t3 = proportionalTime(-1200, p3, animationTime);
        animation1 = Helpers().startAnimation(tree1, 0, p1, t1);
        animation2 = Helpers().startAnimation(tree2, 100, p2, t2);
        animation3 = Helpers().startAnimation(tree3, 180, p3, t3);
        isFirstBatch = false; // Next one should be Batch B
    }

    // start next batch timer
    animationTimer->start(animationTime / 2);
}
// put all the trees offscreen
void GameState::putTreesOffScreen()
{
    tree1->setPos(600, 150);
    tree2->setPos(600, 150);
    tree3->setPos(600, 150);
    tree4->setPos(600, 150);
    tree5->setPos(600, 150);
    tree6->setPos(600, 150);
}
