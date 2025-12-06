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

static const std::array<std::array<int, 3>, 50> combos{
    {{-1150, -850, -450}, {-1100, -900, -500}, {-1050, -750, -350}, {-1000, -650, -250},
     {-950, -550, -150},  {-900, -700, -300},  {-850, -500, -200},  {-800, -600, -400},
     {-750, -450, -150},  {-700, -550, -350},  {-650, -750, -250},  {-600, -500, -300},
     {-550, -850, -400},  {-500, -650, -200},  {-450, -750, -350},  {-400, -600, -150},
     {-350, -700, -250},  {-300, -550, -450},  {-250, -650, -350},  {-200, -750, -400},
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
    auto [p1, p2, p3] = combos[rng() % combos.size()];
    int t1 = proportionalTime(-1200, p1, animationTime);
    int t2 = proportionalTime(-1200, p2, animationTime);
    int t3 = proportionalTime(-1200, p3, animationTime);
    animation1 = Helpers().startAnimation(tree1, 50, p1, t1);
    animation2 = Helpers().startAnimation(tree2, 200, p2, t2);
    animation3 = Helpers().startAnimation(tree3, 350, p3, t3);

    animationTimer = new QTimer();
    QObject::connect(animationTimer, &QTimer::timeout, [this]() {
        auto [p1, p2, p3] = combos[rng() % combos.size()];
        int t1 = proportionalTime(-1200, p1, animationTime);
        int t2 = proportionalTime(-1200, p2, animationTime);
        int t3 = proportionalTime(-1200, p3, animationTime);
        animation1 = Helpers().startAnimation(tree1, 50, p1, t1);
        animation2 = Helpers().startAnimation(tree2, 200, p2, t2);
        animation3 = Helpers().startAnimation(tree3, 350, p3, t3);
        animationTime = std::max(animationTime - 100, 100);
        animationTimer->setInterval(animationTime);
    });
    animationTimer->start(animationTime);
}
