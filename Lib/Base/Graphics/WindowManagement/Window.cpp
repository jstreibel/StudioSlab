//
// Created by joao on 20/04/2021.
//

#include "Window.h"
#include "Base/Graphics/Artists/StylesAndColorSchemes.h"

#include <GL/gl.h>

Window::Window(int x, int y, int w, int h) : w(w), h(h), x(x), y(y) {}

void Window::draw(bool decorated, bool clear) const {
    setupViewport(decorated, clear);

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
        glColor3d(1, 1, 1);
        glVertex2f(-p, -p);
        glVertex2f(-p, p);
        glVertex2f(p, p);
        glVertex2f(p, -p);
    }
    glEnd();
}

void Window::setupViewport(bool decorate, bool clear) const {
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

void Window::reshape(int w, int h) {
    this->w = w;
    this->h = h;

    for(auto artist : content)
        artist->reshape(w, h);

}
