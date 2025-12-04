#include <QDebug>
#include <QPropertyAnimation>

#include "animatableimage.h"

#ifndef HELPERS_H
#define HELPERS_H

class Helpers
{
public:
    // image loader funciton
    static AnimatableImage *loadImage(QString path, int width, int height);
    // Helper to start animation with config values
    static QPropertyAnimation *startAnimation(AnimatableImage *i, int x, int y_offset, int duration);
};

#endif // HELPERS_H
