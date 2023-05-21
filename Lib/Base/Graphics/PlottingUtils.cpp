#include "PlottingUtils.h"

#include <string>

AARect2D::AARect2D(Point2D topLeft, Point2D bottomRight) : topLeft(topLeft), bottomRight(bottomRight) {

}

void AARect2D::setLeft(float xMin) {
    topLeft.x = xMin;
}

void AARect2D::setRight(float xMax) {
    bottomRight.x = xMax;
}

void AARect2D::setTop(float yMax) {
    topLeft.y = yMax;
}

void AARect2D::setBottom(float yMin) {
    bottomRight.y = yMin;
}

auto AARect2D::width() const -> float {
    return bottomRight.x - topLeft.x;
}

auto AARect2D::height() const -> float {
    return topLeft.y - bottomRight.y ;
}
