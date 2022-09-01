//
// Created by joao on 02/09/2021.
//

#include "GLUTEventListener.h"
#include "GLUTUtils.h"

#include <GL/gl.h>

#include <Common/Typedefs.h>



Base::GLUTEventListener::GLUTEventListener() {

}



auto Base::GLUTEventListener::getWindowSizeHint() -> IntPair {
    return {-1,-1};
}

auto Base::GLUTEventListener::finishFrameAndRender() -> bool {
    for(auto *anim : animations) anim->step(frameTimer.getElTime());

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glPushMatrix();
    {
        {
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            glEnable(GL_POINT_SMOOTH);
            glEnable(GL_LINE_SMOOTH);
        }

        if (needDraw())
            draw();
    }
    glPopMatrix();

    frameTimer.reset();
    return true;
}

void Base::GLUTEventListener::SetWindow(double x, double y, double width, double height, const char *title) const {
    const double myx = x;// - winXoffset/1.0;
    const double myy = y;// - winYoffset/1.0;
    const double myw = width;
    const double myh = height;

    glViewport(myx + winXoffset, myy + winYoffset,
               myw-2*winXoffset, myh-2*winYoffset);
    glLoadIdentity();

    // write(-0.95, 0.9, title);

    const double p = 0.999;
    glBegin(GL_QUADS);
    {
        glColor4d(0.1, 0.1, 0.1, 1.0);
        glVertex2f(-p,-p);
        glVertex2f(-p, p);
        glVertex2f( p, p);
        glVertex2f( p,-p);
    }
    glEnd();

    glBegin(GL_LINE_LOOP);
    {
        glColor3d(0, 0, 0);
        glVertex2f(-p,-p);
        glVertex2f(-p, p);
        glVertex2f( p, p);
        glVertex2f( p,-p);
    }
    glEnd();
}

void Base::GLUTEventListener::notifyReshape(int width, int height) {
    windowWidth = width;
    windowHeight = height;
}

auto Base::GLUTEventListener::needDraw() const -> bool {return true;}


