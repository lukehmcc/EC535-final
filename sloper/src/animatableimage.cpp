#include "animatableimage.h"

AnimatableImage::AnimatableImage(const QPixmap &pix)
    : QGraphicsPixmapItem(pix)
{
    // Set origin to center so it spins in place rather than around the top-left
    // corner
    setTransformOriginPoint(pix.width() / 2, pix.height() / 2 - 200);
}
