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
#include "sonarcontrolledview.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QGraphicsScene scene;

    // Load images & make them animatable
    // sources from the build/ directory
    AnimatableImage *guy = Helpers().loadImage("../static/wizard.png", 70, 70);
    scene.addItem(guy);
    guy->setPos(120, 300);

    // tree1
    AnimatableImage *tree1 = Helpers().loadImage("../static/pine.png", 100, 100);
    scene.addItem(tree1);
    tree1->setPos(600, 150); // put tree off screen
    // tree2
    AnimatableImage *tree2 = Helpers().loadImage("../static/pine.png", 100, 100);
    scene.addItem(tree2);
    tree2->setPos(600, 150); // put tree off screen
    // tree 3
    AnimatableImage *tree3 = Helpers().loadImage("../static/pine.png", 100, 100);
    scene.addItem(tree3);
    tree3->setPos(600, 150); // put tree off screen
    // tree 4
    AnimatableImage *tree4 = Helpers().loadImage("../static/pine.png", 100, 100);
    scene.addItem(tree4);
    tree4->setPos(600, 150); // put tree off screen
    // tree 5
    AnimatableImage *tree5 = Helpers().loadImage("../static/pine.png", 100, 100);
    scene.addItem(tree5);
    tree5->setPos(600, 150); // put tree off screen
    // tree 6
    AnimatableImage *tree6 = Helpers().loadImage("../static/pine.png", 100, 100);
    scene.addItem(tree6);
    tree6->setPos(600, 150); // put tree off screen
    // background 1
    AnimatableImage *background1 = Helpers().loadImage("../static/background.png", 272, 960);
    scene.addItem(background1);
    background1->setPos(0, 0);
    background1->setZValue(-1);
    // background 2
    AnimatableImage *background2 = Helpers().loadImage("../static/background.png", 272, 960);
    scene.addItem(background2);
    background2->setPos(300, 0); // put tree off screen
    background2->setZValue(-1);

    // Define the collsion clock
    QTimer *colisionClock = new QTimer(&scene);

    // Score counter cock
    QGraphicsTextItem *timerText = scene.addText("0");
    timerText->setPos(220, 20);
    QGraphicsTextItem *bestScoreText = scene.addText("Best Score: 0");
    bestScoreText->setPos(20, 20);
    QElapsedTimer *elapsed = new QElapsedTimer();
    elapsed->start(); // keeps track of how long since it started

    // title screen
    AnimatableImage *titleImage = Helpers().loadImage("../static/title.png", 272, 480);
    scene.addItem(titleImage);
    titleImage->setPos(0, 0);

    // define the game logic
    GameState *state = new GameState(guy,
                                     tree1,
                                     tree2,
                                     tree3,
                                     tree4,
                                     tree5,
                                     tree6,
                                     background1,
                                     background2,
                                     colisionClock,
                                     elapsed);

    // Start the time based watcher
    QObject::connect(colisionClock, &QTimer::timeout, [&]() {
        // check collision
        for (auto *it : scene.items()) {
            if (it != guy && it != background1 && it != background2 && it != titleImage
                && it->collidesWithItem(guy)) {
                state->stop();
            }
        }
        // add title screen hover
        if (!state->titleScreen && titleImage) {
            titleImage->hide();
        }
        // update timer
        if (!state->paused) {
            timerText->setPlainText(QString::number(elapsed->elapsed() / 100));
        }
        bestScoreText->setPlainText("Best Score: " + QString::number(state->getBestScore() / 100));
    });
    colisionClock->start(10); // start polling

    // set up sonar
    SonarControlledView sonar(state);
    // Setup View with keyboard controls
    KeyboardControlledView view(&scene, state);
    view.setRenderHint(QPainter::Antialiasing);
    scene.setSceneRect(0, 0, 272, 480);
    view.setFixedSize(480, 272); // match sceneRect
    view.setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    view.setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    view.show();
    view.rotate(90);

    return a.exec();
}

// This is needed for now because it's in one main.cpp file
#include "main.moc"
