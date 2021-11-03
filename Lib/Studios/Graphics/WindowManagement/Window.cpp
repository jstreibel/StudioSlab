//
// Created by joao on 20/04/2021.
//

#include "Window.h"

#include <GL/gl.h>

Window::Window(int x, int y, int w, int h) : w(w), h(h), x(x), y(y) {}

void Window::draw(bool decorated, bool clear) const {
    setupViewport(decorated, clear);

    for(auto artist : content) artist->draw(this);
}

void Window::_clear() const {
    glBegin(GL_QUADS);
    {

        glColor4d(0.1, 0.1, 0.1, 1.0);
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
    glMatrixMode(GL_VIEWPORT);
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
