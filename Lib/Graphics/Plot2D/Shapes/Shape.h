//
// Created by joao on 6/8/24.
//

#ifndef STUDIOSLAB_GRAPHICS_SHAPE_H
#define STUDIOSLAB_GRAPHICS_SHAPE_H

#include "Graphics/Types2D.h"

namespace Slab::Graphics {

    class FPlot2DWindow;

    class Shape {
        virtual void draw(const FPlot2DWindow &) = 0;
    };

    class RectangleShape : public Shape {
    public:
        Point2D top_left, bottom_right;

        RectangleShape(Point2D top_left, Point2D bottom_right);

        void draw(const FPlot2DWindow &window) override;
    };

} // Slab::Graphics

#endif // STUDIOSLAB_GRAPHICS_SHAPE_H
