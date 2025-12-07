#include "helpers.h"

// image loader funciton
AnimatableImage *Helpers::loadImage(QString path, int width, int height)
{
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
    return new AnimatableImage(pix);
}

// Helper to start animation with config values
QPropertyAnimation *Helpers::startAnimation(AnimatableImage *i, int x, int y_offset, int duration)
{
    QPropertyAnimation *a = new QPropertyAnimation(i, "pos");
    a->setDuration(duration);               // 5 seconds to move down
    a->setStartValue(QPointF(x, y_offset)); // Start above screen at x=200
    a->setEndValue(QPointF(x, 1150));       // End below screen
    a->setEasingCurve(QEasingCurve::Linear);
    a->start();
    return a;
}
