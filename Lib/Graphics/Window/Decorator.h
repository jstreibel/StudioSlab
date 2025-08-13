//
// Created by joao on 10/20/24.
//

#ifndef STUDIOSLAB_DECORATOR_H
#define STUDIOSLAB_DECORATOR_H

#include "SlabWindow.h"

#include "Graphics/OpenGL/WriterOpenGL.h"

namespace Slab::Graphics {

    class FDecorator {
        int SysWin_Width=0, SysWin_Height=0;

        OpenGL::FWriterOpenGL Writer;

        void Setup() const;

    public:
        virtual ~FDecorator() = default;
        FDecorator();

        virtual
        void BeginDecoration(const FSlabWindow&, int x_mouse, int y_mouse);

        virtual
        void FinishDecoration(const FSlabWindow&, int x_mouse, int y_mouse);

        void SetSystemWindowShape(int w, int h);

        virtual
        bool IsMouseOverTitlebar(const FSlabWindow&, int x_mouse, int y_mouse);

        virtual
        bool IsMouseOverCorner(const FSlabWindow&, int x_mouse, int y_mouse);

        virtual
        bool IsMouseOverGrabRegion(const FSlabWindow&, int x_mouse, int y_mouse);

        int TitlebarHeight();
    };

} // Slab::Graphics

#endif //STUDIOSLAB_DECORATOR_H
