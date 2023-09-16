//
// Created by joao on 20/04/2021.
//

#include "Core/Graphics/OpenGL/OpenGL.h"

#include "Window.h"
#include "Core/Graphics/Styles/WindowStyles.h"
#include "Core/Graphics/OpenGL/GLUTUtils.h"
#include "Core/Backend/GUIBackend.h"


Window::Window(int x, int y, int w, int h) : windowRect(x, x+w, y, y+h) {}

void Window::draw() {
    glUseProgram( 0 );

    setupWindow();

    for(auto artist : content) artist->draw(this);
}

void Window::_clear() const {

    auto &bg = backgroundColor;

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
        auto bc = isMouseIn() ? Core::Graphics::windowBorderColor_active
                              : Core::Graphics::windowBorderColor_inactive;

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
    glViewport(vp.xMin-2, vp.yMin-2, vp.width()+4, vp.height() + 4);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    if (clear)
        this->_clear();
    if (decorate)
        this->_decorate();

    glViewport(vp.xMin, vp.yMin, vp.width(), vp.height());

}

void Window::addArtist(Artist *pArtist) {
    content.emplace_back(pArtist);
}

auto Window::isMouseIn() const -> bool {
    fix &mouse = GUIBackend::GetInstance().getMouseState();
    auto hScreen = GUIBackend::GetInstance().getScreenHeight();

    fix x = windowRect.xMin;
    fix y = windowRect.yMin;
    fix w = windowRect.width();
    fix h = windowRect.height();

    return mouse.x > x && mouse.x < x + w && hScreen - mouse.y > y && hScreen - mouse.y < y + h;
}

auto Window::getMouseWindowCoord() const -> Point2D {
    fix &mouse = GUIBackend::GetInstance().getMouseState();

    fix xMouseLocal =      mouse.x - windowRect.xMin;
    fix yMouseLocal = 1 - (mouse.y - windowRect.yMin);

    return {(Real)xMouseLocal, (Real)yMouseLocal};
}

auto Window::getMouseViewportCoord() const -> Point2D {
    fix &mouse = GUIBackend::GetInstance().getMouseState();
    fix hScreen = GUIBackend::GetInstance().getScreenHeight();
    auto vpRect = getViewport();

    fix xMouseLocal =         mouse.x - vpRect.xMin;
    fix yMouseLocal = hScreen-mouse.y - vpRect.yMin;

    return {(Real)xMouseLocal, (Real)yMouseLocal};
}

bool Window::notifyScreenReshape(int newScreenWidth, int newScreenHeight) {
    EventListener::notifyScreenReshape(newScreenWidth, newScreenHeight);

    this->notifyReshape(newScreenWidth, newScreenHeight);

    return true;
}

void Window::notifyReshape(int w, int h) {
    this->setSize(w, h);

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
    auto _x = getx() +     Core::Graphics::hPadding,
         _y = gety() +     Core::Graphics::vPadding,
         _w = getw() - 2 * Core::Graphics::hPadding,
         _h = geth() - 2 * Core::Graphics::vPadding;

    return {_x, _x+_w, _y, _y+_h};
}
