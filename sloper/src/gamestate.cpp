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
    bestScore = 0; // init
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
    if (animation4) {
        animation4->stop();
    }
    animationTimer->stop();
    if (animation5) {
        animation5->stop();
    }
    if (animation6) {
        animation6->stop();
    }
    colTimer->stop();
    if (elapsed->elapsed() > bestScore) {
        bestScore = elapsed->elapsed();
    }
}
// Resets the game to intial state (doesn't clear clock)
void GameState::reset()
{
    paused = false;
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
    animationTime = 10000; // 10s
    // First batch (0, 200, 350)
    auto [p1, p2, p3] = combos[rng() % combos.size()];
    int t1 = proportionalTime(-1200, p1, animationTime);
    int t2 = proportionalTime(-1200, p2, animationTime);
    int t3 = proportionalTime(-1200, p3, animationTime);
    animation1 = Helpers().startAnimation(tree1, 50, p1, t1);
    animation2 = Helpers().startAnimation(tree2, 200, p2, t2);
    animation3 = Helpers().startAnimation(tree3, 350, p3, t3);

    // Second batch offset by half
    QTimer::singleShot(animationTime / 2, [this]() {
        auto [p4, p5, p6] = combos[rng() % combos.size()];
        int t4 = proportionalTime(-1200, p4, animationTime);
        int t5 = proportionalTime(-1200, p5, animationTime);
        int t6 = proportionalTime(-1200, p6, animationTime);
        animation4 = Helpers().startAnimation(tree4, 50, p4, t4);
        animation5 = Helpers().startAnimation(tree5, 200, p5, t5);
        animation6 = Helpers().startAnimation(tree6, 350, p6, t6);
    });

    // Timer alternates between the two groups
    bool useFirst = true;
    animationTimer = new QTimer();
    QObject::connect(animationTimer, &QTimer::timeout, [this, &useFirst]() {
        if (useFirst) {
            auto [p1, p2, p3] = combos[rng() % combos.size()];
            int t1 = proportionalTime(-1200, p1, animationTime);
            int t2 = proportionalTime(-1200, p2, animationTime);
            int t3 = proportionalTime(-1200, p3, animationTime);
            animation1 = Helpers().startAnimation(tree1, 50, p1, t1);
            animation2 = Helpers().startAnimation(tree2, 200, p2, t2);
            animation3 = Helpers().startAnimation(tree3, 350, p3, t3);
        } else {
            auto [p4, p5, p6] = combos[rng() % combos.size()];
            int t4 = proportionalTime(-1200, p4, animationTime);
            int t5 = proportionalTime(-1200, p5, animationTime);
            int t6 = proportionalTime(-1200, p6, animationTime);
            animation4 = Helpers().startAnimation(tree4, 50, p4, t4);
            animation5 = Helpers().startAnimation(tree5, 200, p5, t5);
            animation6 = Helpers().startAnimation(tree6, 350, p6, t6);
        }
        useFirst = !useFirst;
        animationTime = std::max(animationTime - 100, 100);
        animationTimer->setInterval(animationTime / 2);
    });
    animationTimer->start(animationTime / 2);
}
