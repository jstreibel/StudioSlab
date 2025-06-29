//
// Created by joao on 10/20/24.
//

#ifndef STUDIOSLAB_DECORATOR_H
#define STUDIOSLAB_DECORATOR_H

#include "SlabWindow.h"

#include "Graphics/OpenGL/WriterOpenGL.h"

namespace Slab::Graphics {

    class Decorator {
        int syswin_w=0, syswin_h=0;

        OpenGL::WriterOpenGL writer;

        void setup() const;

    public:
        Decorator();

        virtual
        void begin_decoration(const FSlabWindow&, int x_mouse, int y_mouse);

        virtual
        void finish_decoration(const FSlabWindow&, int x_mouse, int y_mouse);

        void setSystemWindowShape(int w, int h);

        virtual
        bool isMouseOverTitlebar(const FSlabWindow&, int x_mouse, int y_mouse);

        virtual
        bool isMouseOverCorner(const FSlabWindow&, int x_mouse, int y_mouse);

        virtual
        bool isMouseOverGrabRegion(const FSlabWindow&, int x_mouse, int y_mouse);

        int titlebar_height();
    };

} // Slab::Graphics

#endif //STUDIOSLAB_DECORATOR_H
