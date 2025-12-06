#include <QApplication>
#include <QDebug> // For checking if image loaded
#include <QElapsedTimer>
#include <QGraphicsPixmapItem>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QKeyEvent>
#include <QPropertyAnimation>
#include <QTimer>

#include "animatableimage.h"
#include "gamestate.h"
#include "helpers.h"
#include "keyboardcontrolledview.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QGraphicsScene scene;

    // Load images & make them animatable
    // sources from the build/ directory
    AnimatableImage *guy = Helpers().loadImage("../static/ski.jpg", 150, 150);
    scene.addItem(guy);
    guy->setPos(200, 700);

    // tree1
    AnimatableImage *tree1 = Helpers().loadImage("../static/pine.png", 150, 150);
    scene.addItem(tree1);
    tree1->setPos(600, 150); // put tree off screen

    // tree2
    AnimatableImage *tree2 = Helpers().loadImage("../static/pine.png", 150, 150);
    scene.addItem(tree2);
    tree2->setPos(600, 150); // put tree off screen

    // tree 3
    AnimatableImage *tree3 = Helpers().loadImage("../static/pine.png", 150, 150);
    scene.addItem(tree3);
    tree3->setPos(600, 150); // put tree off screen

    // Define the collsion clock
    QTimer *colisionClock = new QTimer(&scene);

    // Score counter cock
    QGraphicsTextItem *timerText = scene.addText("0");
    timerText->setPos(420, 10);
    QGraphicsTextItem *bestScoreText = scene.addText("Best Score: 0");
    bestScoreText->setPos(300, 10);
    QElapsedTimer *elapsed = new QElapsedTimer();
    elapsed->start(); // keeps track of how long since it started

    // define the game logic
    GameState *state = new GameState(guy, tree1, tree2, tree3, colisionClock, elapsed);

    // Start the time based watcher
    QObject::connect(colisionClock, &QTimer::timeout, [&]() {
        // check collision
        for (auto *it : scene.items()) {
            if (it != guy && it->collidesWithItem(guy)) {
                state->stop();
            }
        }
        // update timer
        timerText->setPlainText(QString::number(elapsed->elapsed() / 100));
        bestScoreText->setPlainText("Best Score: " + QString::number(state->getBestScore() / 100));
    });
    colisionClock->start(10); // start polling

    // Setup View with keyboard controls
    KeyboardControlledView view(&scene, state);
    view.setRenderHint(QPainter::Antialiasing);
    scene.setSceneRect(0, 0, 500, 1000);
    view.setFixedSize(500, 1000); // match sceneRect
    view.setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    view.setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    view.show();

    return a.exec();
}

// This is needed for now because it's in one main.cpp file
#include "main.moc"
