//
// Created by joao on 20/04/2021.
//

#include "Core/Graphics/OpenGL/OpenGL.h"

#include "Window.h"
#include "Core/Graphics/Styles/StylesAndColorSchemes.h"
#include "Core/Graphics/Styles/WindowStyles.h"


Window::Window(int x, int y, int w, int h) : w(w), h(h), x(x), y(y) {}

void Window::draw() {
    glUseProgram( 0 );

    setupWindow();

    for(auto artist : content) artist->draw(this);
}

void Window::_clear() const {

    auto &bg = Styles::GetColorScheme()->windowBackground;

    glBegin(GL_QUADS);
    {
        glColor4d(bg.r, bg.g, bg.b, bg.a);
        glVertex2f(-p, -p);
        glVertex2f( p, -p);
        glVertex2f( p,  p);
        glVertex2f(-p,  p);
    }
    glEnd();
}

void Window::_decorate() const {
    glBegin(GL_LINE_LOOP);
    {
        auto bc = Styles::GetColorScheme()->windowBorder;
        glColor4d(bc.r, bc.g, bc.b, bc.a);

        glVertex2f(-p, -p);
        glVertex2f( p, -p);
        glVertex2f( p,  p);
        glVertex2f(-p,  p);
    }
    glEnd();
}

void Window::setupWindow() const {
    glEnable(GL_LINE_SMOOTH);
    glDisable(GL_LINE_STIPPLE);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    auto vp = getViewport();
    glViewport(vp.xMin, vp.yMin, vp.w(), vp.h());

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    if (clear)
        this->_clear();
    if (decorate)
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

    this->notifyReshape(newScreenWidth, newScreenHeight);

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

bool Window::notifyRender(float elTime_msec) {
    this->draw();

    return EventListener::notifyRender(elTime_msec);
}

RectI Window::getViewport() const {
    auto _x = x +     Core::Graphics::hPadding,
         _y = y +     Core::Graphics::vPadding,
         _w = w - 2 * Core::Graphics::hPadding,
         _h = h - 2 * Core::Graphics::vPadding;

    return {_x, _x+_w, _y, _y+_h};
}
