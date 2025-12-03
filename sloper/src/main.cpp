#include <QApplication>
#include <QDebug> // For checking if image loaded
#include <QGraphicsPixmapItem>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QKeyEvent>
#include <QPropertyAnimation>
#include <QTimer>

// Define the Wrapper Class
// This is needed to add animation to a sprite
class AnimatableImage : public QObject, public QGraphicsPixmapItem {
  Q_OBJECT
  // These macros allow QPropertyAnimation to "see" these values
  Q_PROPERTY(QPointF pos READ pos WRITE setPos)
  Q_PROPERTY(qreal rotation READ rotation WRITE setRotation)

public:
  explicit AnimatableImage(const QPixmap &pix) : QGraphicsPixmapItem(pix) {
    // Set origin to center so it spins in place rather than around the top-left
    // corner
    setTransformOriginPoint(pix.width() / 2, pix.height() / 2 - 200);
  }
};

// class to handle keyboard input
class KeyboardControlledView : public QGraphicsView {
  Q_OBJECT

public:
  KeyboardControlledView(QGraphicsScene *scene, AnimatableImage *skier)
      : QGraphicsView(scene), m_skier(skier) {
    // enable keyboard focus
    setFocusPolicy(Qt::StrongFocus);
    // mvement step size (in pixels)
    m_stepSize = 150.0;
  }

protected:
    void keyPressEvent(QKeyEvent *event) override
    {
        // get current position
        QPointF currentPos = m_skier->pos();
        QPointF newPos = currentPos;

        // handle arrow key movement
        switch (event->key()) {
        case Qt::Key_Up:
            if (is_bottom) {
                is_bottom = false;
                newPos.setY(currentPos.y() - m_stepSize);
            } else if (!is_top) {
                is_top = true;
                newPos.setY(currentPos.y() - m_stepSize);
            }
            break;
        case Qt::Key_Down:
            if (is_top) {
                is_top = false;
                newPos.setY(currentPos.y() + m_stepSize);
            } else if (!is_bottom) {
                is_bottom = true;
                newPos.setY(currentPos.y() + m_stepSize);
            }
            break;
        case Qt::Key_Left:
            if (is_right) {
                is_right = false;
                newPos.setX(currentPos.x() - m_stepSize);
            } else if (!is_left) {
                is_left = true;
                newPos.setX(currentPos.x() - m_stepSize);
            }
            break;
        case Qt::Key_Right:
            if (is_left) {
                is_left = false;
                newPos.setX(currentPos.x() + m_stepSize);
            } else if (!is_right) {
                is_right = true;
                newPos.setX(currentPos.x() + m_stepSize);
            }
            break;
        default:
            QGraphicsView::keyPressEvent(event);
            return;
        }

        // update skier position
        m_skier->setPos(newPos);
    }

private:
  AnimatableImage *m_skier;
  qreal m_stepSize;
  bool is_left = false;
  bool is_right = false;
  bool is_top = false;
  bool is_bottom = false;
};

// image loader funciton
AnimatableImage *loadImage(QString path, int width, int height) {
  QPixmap pix(path);

  if (pix.isNull()) {
    qWarning() << "Image failed to load! Check your path.";
    // Fallback: create a colored square so you can still see the animation
    // work
    pix = QPixmap(100, 100);
    pix.fill(Qt::red);
  } else {
    // make sure to scale it down to fir the canvas
    pix = pix.scaled(QSize(width, height), Qt::KeepAspectRatio,
                     Qt::FastTransformation);
  }
  return new AnimatableImage(pix);
}

class GameState {
public:
    GameState(AnimatableImage *g,
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
    AnimatableImage *guy;
    AnimatableImage *tree1;
    AnimatableImage *tree2;
    AnimatableImage *tree3;
    QPropertyAnimation *animation1;
    QPropertyAnimation *animation2;
    QPropertyAnimation *animation3;
    QTimer *colTimer;
    bool isPaused() { return paused; }
    // Function that stops the game
    void stop()
    {
        paused = true;
        animation1->stop();
        animation2->stop();
        animation3->stop();
        colTimer->stop();
    }
    // Resets the game to intial state (doesn't clear clock)
    void reset() { paused = false; }

private:
    bool paused;
};

// Helper to start animation with config values
QPropertyAnimation *startAnimation(AnimatableImage *i, int x, int y_offset, int duration)
{
    QPropertyAnimation *a = new QPropertyAnimation(i, "pos");
    a->setDuration(duration);               // 5 seconds to move down
    a->setStartValue(QPointF(x, y_offset)); // Start above screen at x=200
    a->setEndValue(QPointF(x, 1150));       // End below screen
    a->setEasingCurve(QEasingCurve::Linear);
    a->setLoopCount(-1);
    a->start(QAbstractAnimation::DeleteWhenStopped);
    return a;
}

int main(int argc, char *argv[]) {
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
