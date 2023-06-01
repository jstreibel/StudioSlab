//
// Created by joao on 20/04/2021.
//

#include "Window.h"
#include "Base/Graphics/Artists/StylesAndColorSchemes.h"
#include "Base/Graphics/OpenGL/GLUTUtils.h"

#include <GL/gl.h>

Window::Window(int x, int y, int w, int h) : w(w), h(h), x(x), y(y) {}

void Window::draw(bool decorated, bool clear) {
    _setupViewport(decorated, clear);

    for(auto artist : content) artist->draw(this);
}

void Window::_clear() const {

    auto &bg = Styles::GetColorScheme()->background;

    glBegin(GL_QUADS);
    {
        glColor4d(bg.r, bg.g, bg.b, bg.a);
        glVertex2f(-p, -p);
        glVertex2f(-p, p);
        glVertex2f(p, p);
        glVertex2f(p, -p);
    }
    glEnd();
}

void Window::_decorate() const {
    glBegin(GL_LINE_LOOP);
    {
        if(gotHit)  glColor3d(1, 0, 0);
        else        glColor3d(1, 1, 1);

        glVertex2f(-p, -p);
        glVertex2f(-p, p);
        glVertex2f(p, p);
        glVertex2f(p, -p);
    }
    glEnd();
}

void Window::_setupViewport(bool decorate, bool clear) const {
    glEnable(GL_LINE_SMOOTH);
    glDisable(GL_LINE_STIPPLE);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    glViewport(x + winXoffset, y + winYoffset,
               w-2*winXoffset, h-2*winYoffset);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    if(clear)
        this->_clear();

    if(decorate)
        this->_decorate();
}

void Window::addArtist(Artist *pArtist) {
    content.emplace_back(pArtist);
}

auto Window::hit(int _x, int _y) const -> bool {
    auto hScreen = GLUTUtils::getScreenHeight();

    const_cast<bool&>(gotHit) = _x > x && _x < x+w && hScreen-_y > y && hScreen-_y < y+h;
    return gotHit;
}

void Window::notifyScreenReshape(int newScreenWidth, int newScreenHeight) {
    GLUTEventListener::notifyScreenReshape(newScreenWidth, newScreenHeight);

    this->w = newScreenWidth;
    this->h = newScreenHeight;

    for(auto artist : content)
        artist->reshape(w, h);
}

void Window::notifyReshape(int newWinW, int newWinH) {
    this->w = newWinW;
    this->h = newWinH;

    for(auto artist : content)
        artist->reshape(w, h);
}
