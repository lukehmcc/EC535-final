#include <QApplication>
#include <QDebug> // For checking if image loaded
#include <QGraphicsPixmapItem>
#include <QGraphicsScene>
#include <QGraphicsView>
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
    setTransformOriginPoint(pix.width() / 2, pix.height() / 2);
  }
};

int main(int argc, char *argv[]) {
  QApplication a(argc, argv);

  QGraphicsScene scene;

  // Load image
  // sources from the build/ directory
  QPixmap pix("../static/ski.jpg");

  if (pix.isNull()) {
    qWarning() << "Image failed to load! Check your path.";
    // Fallback: create a colored square so you can still see the animation
    // work
    pix = QPixmap(100, 100);
    pix.fill(Qt::red);
  }

  // Create the item and add to scene
  AnimatableImage *guy = new AnimatableImage(pix);
  scene.addItem(guy);

  // Setup Animation: Let's Move AND Rotate him

  // Animation A: Movement
  QPropertyAnimation *moveAnim = new QPropertyAnimation(guy, "pos");
  moveAnim->setDuration(3000);
  moveAnim->setStartValue(QPointF(0, 0));
  moveAnim->setEndValue(QPointF(200, 200));
  moveAnim->setLoopCount(-1); // Loop forever
  moveAnim->start();

  // Animation B: Rotation (Spinning)
  QPropertyAnimation *spinAnim = new QPropertyAnimation(guy, "rotation");
  spinAnim->setDuration(1000);
  spinAnim->setStartValue(0);
  spinAnim->setEndValue(360);
  spinAnim->setLoopCount(-1);
  spinAnim->start();

  // 5. Setup View
  QGraphicsView view(&scene);
  view.setRenderHint(QPainter::Antialiasing);
  view.resize(600, 400);
  view.show();

  return a.exec();
}

// This is needed for now because it's in one main.cpp file
#include "main.moc"
