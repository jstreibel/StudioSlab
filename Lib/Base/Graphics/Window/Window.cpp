//
// Created by joao on 20/04/2021.
//

#include "Window.h"
#include "Base/Graphics/Styles/StylesAndColorSchemes.h"
#include "Base/Graphics/OpenGL/GLUTUtils.h"

#include <GL/gl.h>

Window::Window(int x, int y, int w, int h) : w(w), h(h), x(x), y(y) {}

void Window::draw() {
    _setupViewport();

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
        if(gotHit)  glColor4d(1, 1, 1, .9);
        else        glColor4d(1, 1, 1, 1);

        glVertex2f(-p, -p);
        glVertex2f(-p, p);
        glVertex2f(p, p);
        glVertex2f(p, -p);
    }
    glEnd();
}

void Window::_setupViewport() const {
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

auto Window::doesHit(int xMouse, int yMouse) const -> bool {
    auto hScreen = GLUTUtils::getScreenHeight();

    const_cast<bool&>(gotHit) = xMouse > x && xMouse < x + w && hScreen - yMouse > y && hScreen - yMouse < y + h;
    return gotHit;
}

bool Window::notifyScreenReshape(int newScreenWidth, int newScreenHeight) {
    EventListener::notifyScreenReshape(newScreenWidth, newScreenHeight);


    this->w = newScreenWidth;
    this->h = newScreenHeight;

    for(auto artist : content)
        artist->reshape(w, h);

    return true;
}

void Window::notifyReshape(int newWinW, int newWinH) {
    this->w = newWinW;
    this->h = newWinH;

    for(auto artist : content)
        artist->reshape(w, h);
}

IntPair Window::getWindowSizeHint() { return {-1, -1}; }

void Window::setDecorate(bool _decorate) { this->decorate = _decorate; }

void Window::setClear(bool _clear) { this->clear = _clear; }
