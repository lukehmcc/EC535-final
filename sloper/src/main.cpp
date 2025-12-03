#include <QApplication>
#include <QDebug> // For checking if image loaded
#include <QGraphicsPixmapItem>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QKeyEvent>
#include <QPropertyAnimation>
#include <QTimer>

#include "animatableimage.h"
#include "gamestate.h"
#include "keyboardcontrolledview.h"

AnimatableImage *loadImage(QString path, int width, int height);
QPropertyAnimation *startAnimation(AnimatableImage *i, int x, int y_offset, int duration);

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QGraphicsScene scene;

    // Load images & make them animatable
    // sources from the build/ directory
    AnimatableImage *guy = loadImage("../static/ski.jpg", 150, 150);
    scene.addItem(guy);
    guy->setPos(200, 700);

    // tree1
    AnimatableImage *tree1 = loadImage("../static/pine.png", 150, 150);
    scene.addItem(tree1);
    tree1->setPos(600, 150); // put tree off screen
    QPropertyAnimation *animation1 = startAnimation(tree1, 50, -850, 8000);

    // tree2
    AnimatableImage *tree2 = loadImage("../static/pine.png", 150, 150);
    scene.addItem(tree2);
    tree2->setPos(600, 150); // put tree off screen
    QPropertyAnimation *animation2 = startAnimation(tree2, 200, -150, 5000);

    // tree 3
    AnimatableImage *tree3 = loadImage("../static/pine.png", 150, 150);
    scene.addItem(tree3);
    tree3->setPos(600, 150); // put tree off screen
    QPropertyAnimation *animation3 = startAnimation(tree3, 350, -1200, 12000);

    // Define the collsion clock
    QTimer *colisionClock = new QTimer(&scene);

    // define the game logic
    GameState *state
        = new GameState(guy, tree1, tree2, tree3, colisionClock, animation1, animation2, animation3);

    // Start the collision checker
    QObject::connect(colisionClock, &QTimer::timeout, [&]() {
        for (auto *it : scene.items()) {
            if (it != guy && it->collidesWithItem(guy)) {
                state->stop();
            }
        }
    });
    colisionClock->start(10); // start polling

    // Setup View with keyboard controls
    KeyboardControlledView view(&scene, guy);
    view.setRenderHint(QPainter::Antialiasing);
    scene.setSceneRect(0, 0, 500, 1000);
    view.show();

    return a.exec();
}

// This is needed for now because it's in one main.cpp file
#include "main.moc"
