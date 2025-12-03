#include <QGraphicsPixmapItem>

#ifndef ANIMATABLEIMAGE_H
#define ANIMATABLEIMAGE_H

// Define the Wrapper Class
// This is needed to add animation to a sprite
class AnimatableImage : public QObject, public QGraphicsPixmapItem
{
    Q_OBJECT
    // These macros allow QPropertyAnimation to "see" these values
    Q_PROPERTY(QPointF pos READ pos WRITE setPos)
    Q_PROPERTY(qreal rotation READ rotation WRITE setRotation)

public:
    explicit AnimatableImage(const QPixmap &pix);
};

#endif // ANIMATABLEIMAGE_H
