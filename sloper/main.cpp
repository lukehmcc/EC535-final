#include <QApplication>
#include <QDebug> // For checking if image loaded
#include <QGraphicsPixmapItem>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QKeyEvent>
#include <QPropertyAnimation>

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
      : QGraphicsView(scene), m_skier(skier), m_animationsPaused(false) {
    // enable keyboard focus
    setFocusPolicy(Qt::StrongFocus);
    // mvement step size (in pixels)
    m_stepSize = 150.0;
  }

protected:
  void keyPressEvent(QKeyEvent *event) override {
    // pause animations on first key press if not already paused
    if (!m_animationsPaused) {
      m_animationsPaused = true;
    }

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
        if (is_right){
            is_right = false;
            newPos.setX(currentPos.x() - m_stepSize);
        }
        else if (!is_left){
            is_left = true;
            newPos.setX(currentPos.x() - m_stepSize);
        }
      break;
    case Qt::Key_Right:
        if (is_left){
            is_left = false;
            newPos.setX(currentPos.x() + m_stepSize);
        }
        else if (!is_right){
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
  bool m_animationsPaused;
  qreal m_stepSize;
  bool is_left = false;
  bool is_right = false;
  bool is_top = false;
  bool is_bottom = false;
};

// image loader funciton
QPixmap loadImage(QString path, int width, int height){
  QPixmap pix(path);

  if (pix.isNull()) {
    qWarning() << "Image failed to load! Check your path.";
    // Fallback: create a colored square so you can still see the animation
    // work
    pix = QPixmap(100, 100);
    pix.fill(Qt::red);
  } else {
      // make sure to scale it down to fir the canvas
      pix = pix.scaled(QSize(width, height), Qt::KeepAspectRatio, Qt::FastTransformation);
  }
  return pix;
}

int main(int argc, char *argv[]) {
  QApplication a(argc, argv);

  QGraphicsScene scene;

  // Load image
  // sources from the build/ directory
  QPixmap pix= loadImage("../static/ski.jpg", 200, 200);
  // Create the item and add to scene
  AnimatableImage *guy = new AnimatableImage(pix);
  scene.addItem(guy);

  // Setup View with keyboard controls
  KeyboardControlledView view(&scene, guy);
  view.setRenderHint(QPainter::Antialiasing);
  view.resize(600, 900);
  view.show();

  return a.exec();
}

// This is needed for now because it's in one main.cpp file
#include "main.moc"

// Animation Examples
  // // Animation A: Movement
  // QPropertyAnimation *moveAnim = new QPropertyAnimation(guy, "pos");
  // moveAnim->setDuration(3000);
  // moveAnim->setStartValue(QPointF(0, 0));
  // moveAnim->setEndValue(QPointF(200, 200));
  // moveAnim->setLoopCount(-1); // Loop forever
  // moveAnim->start();

  // // Animation B: Rotation (Spinning)
  // QPropertyAnimation *spinAnim = new QPropertyAnimation(guy, "rotation");
  // spinAnim->setDuration(1000);
  // spinAnim->setStartValue(0);
  // spinAnim->setEndValue(360);
  // spinAnim->setLoopCount(-1);
  // spinAnim->start();
