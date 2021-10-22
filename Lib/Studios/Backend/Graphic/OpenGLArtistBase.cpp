//
// Created by joao on 02/09/2021.
//

#include "OpenGLArtistBase.h"
#include "GL/gl.h"

#include "View/Graphic/Tools/PlottingUtils.h"



Base::OpenGLArtistBase::OpenGLArtistBase() {

}



auto Base::OpenGLArtistBase::getWindowSizeHint() -> IntPair {
    return {-1,-1};
}

auto Base::OpenGLArtistBase::finishFrameAndRender() -> bool {
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
        renderStats();
    }
    glPopMatrix();

    frameTimer.reset();
    return true;
}

void Base::OpenGLArtistBase::SetWindow(double x, double y, double width, double height, const char *title) const {
    const double myx = x;// - winXoffset/1.0;
    const double myy = y;// - winYoffset/1.0;
    const double myw = width;
    const double myh = height;

    glViewport(myx + winXoffset, myy + winYoffset,
               myw-2*winXoffset, myh-2*winYoffset);
    glLoadIdentity();

    write(-0.95, 0.9, title);

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


void Base::OpenGLArtistBase::renderStats()
{
    SetWindow(0, 0, statsWindowWidth, statsWindowHeight);

    glLineWidth(1);

    float scaleX = 2.f/statsWindowWidth, scaleY = 2.f*2.f/float(windowHeight);
    glScalef(scaleX, scaleY, 1);
    glTranslatef(-statsWindowWidth/2., windowHeight/4., 0);

    glBegin(GL_LINES);
    {
        const float s = 20;
        glVertex2f(-s, 0);
        glVertex2f(s, 0);

        glVertex2f(0, -s);
        glVertex2f(0, s);
    }
    glEnd();


    {
        const int cursorx = 14;
        const float fontHeight = 54;
        float delta = fontHeight;
        auto font = FONT9;

        glColor3b(0.9*255,0.9*255,0.9*255);

        for(auto stat : stats){
            delta += fontHeight;
            write(cursorx, -delta, stat, font);
        }
        stats.clear();
    }
    //glPopMatrix();

}

void Base::OpenGLArtistBase::reshape(int width, int height) {
    windowWidth = width;
    windowHeight = height;
}

auto Base::OpenGLArtistBase::needDraw() const -> bool {return true;}


