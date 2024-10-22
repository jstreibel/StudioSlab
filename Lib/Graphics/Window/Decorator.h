//
// Created by joao on 10/20/24.
//

#ifndef STUDIOSLAB_DECORATOR_H
#define STUDIOSLAB_DECORATOR_H

#include "SlabWindow.h"

namespace Slab::Graphics {

    class Decorator {
        int syswin_w=0, syswin_h=0;

    public:
        virtual
        void decorate(const SlabWindow&, int x_mouse, int y_mouse);

        void setSystemWindowShape(int w, int h);

        virtual
        bool isMouseOverTitlebar(const SlabWindow&, int x_mouse, int y_mouse);

        virtual
        bool isMouseOverCorner(const SlabWindow&, int x_mouse, int y_mouse);

        virtual
        bool isMouseOverGrabRegion(const SlabWindow&, int x_mouse, int y_mouse);
    };

} // Slab::Graphics

#endif //STUDIOSLAB_DECORATOR_H
