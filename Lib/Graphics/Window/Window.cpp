//
// Created by joao on 20/04/2021.
//

#include "Graphics/OpenGL/OpenGL.h"

#include "Graphics/SlabGraphics.h"

#include "Window.h"
#include "WindowStyles.h"
#include "Graphics/OpenGL/GLUTUtils.h"
#include "Graphics/OpenGL/Shader.h"
#include "Core/Tools/Log.h"


namespace Slab::Graphics {

    #define USE_GLOBAL_MOUSECLICK_POLICY false

    Window::Window(int x, int y, int w, int h, Flags flags)
            : backgroundColor(Graphics::clearColor), flags(flags),
              windowRect(x, x + w, y, y + h) {}

    Window::~Window() = default;

    void Window::draw() { setupWindow(); }

    void Window::setupWindow() const {
        OpenGL::Shader::remove();
        glDisable(GL_TEXTURE_2D);

        glEnable(GL_LINE_SMOOTH);
        glDisable(GL_LINE_STIPPLE);

        glDisable(GL_DEPTH_TEST);
        glDepthMask(GL_FALSE);

        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();

        auto vp = getEffectiveViewport();
        glViewport(vp.xMin - 2, vp.yMin - 2, vp.width() + 4, vp.height() + 4);

        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();

        if (clear)
            this->_clear();
        if (decorate)
            this->_decorate();

        glViewport(vp.xMin, vp.yMin, vp.width(), vp.height());

    }

    void Window::_clear() const {
        auto &bg = backgroundColor;

        glBegin(GL_QUADS);
        {
            glColor4d(bg.r, bg.g, bg.b, bg.a);
            glVertex2f(-p, -p);
            glVertex2f(p, -p);
            glVertex2f(p, p);
            glVertex2f(-p, p);
        }
        glEnd();
    }

    void Window::_decorate() const {
        glLineWidth(2.0f);
        glBegin(GL_LINE_LOOP);
        {
            auto bc = isMouseIn() ? Graphics::windowBorderColor_active
                                  : Graphics::windowBorderColor_inactive;

            glColor4d(bc.r, bc.g, bc.b, bc.a);

            glVertex2f(-p, -p);
            glVertex2f(p, -p);
            glVertex2f(p, p);
            glVertex2f(-p, p);
        }
        glEnd();
    }

    auto Window::isMouseIn() const -> bool {
        auto &guiBackend = Slab::Graphics::GetGraphicsBackend();

        fix &mouse = guiBackend.getMouseState();
        auto hScreen = guiBackend.getScreenHeight();

        auto rect = getEffectiveViewport();

        fix x = rect.xMin;
        fix y = rect.yMin;
        fix w = rect.width();
        fix h = rect.height();

        return mouse.x > x && mouse.x < x + w && hScreen - mouse.y > y && hScreen - mouse.y < y + h;
    }

    auto Window::isMouseLeftClicked() const -> bool {
#if USE_GLOBAL_MOUSECLICK_POLICY==true
        auto &guiBackend = Slab::Graphics::GetGraphicsBackend();

        fix &mouse = guiBackend.getMouseState();
        return mouse.leftPressed;
#else
        return mouseLeftButton==KeyState::Press;
#endif
    }

    auto Window::isMouseCenterClicked() const -> bool {
#if USE_GLOBAL_MOUSECLICK_POLICY==true
        auto &guiBackend = Slab::Graphics::GetGraphicsBackend();

        fix &mouse = guiBackend.getMouseState();

        if(!mouse.centerPressed) return false;
#else
        return mouseCenterButton==KeyState::Press;
#endif
    }

    auto Window::isMouseRightClicked() const -> bool {
#if USE_GLOBAL_MOUSECLICK_POLICY==true
        auto &guiBackend = Slab::Graphics::GetGraphicsBackend();

        fix &mouse = guiBackend.getMouseState();

        if(!mouse.rightPressed) return false;
#else
        return mouseRightButton==KeyState::Press;
#endif
    }

    auto Window::getMouseWindowCoord() const -> Point2D {
        fix &mouse = Slab::Graphics::GetGraphicsBackend().getMouseState();

        fix xMouseLocal = mouse.x - windowRect.xMin;
        fix yMouseLocal = 1 - (mouse.y - windowRect.yMin);

        return {(Real) xMouseLocal, (Real) yMouseLocal};
    }

    auto Window::getMouseViewportCoord() const -> Point2D {
        auto &guiBackend = Slab::Graphics::GetGraphicsBackend();

        fix &mouse = guiBackend.getMouseState();
        fix hScreen = guiBackend.getScreenHeight();
        auto vpRect = getEffectiveViewport();

        fix xMouseLocal = mouse.x - vpRect.xMin;
        fix yMouseLocal = hScreen - mouse.y - vpRect.yMin;

        return {(Real) xMouseLocal, (Real) yMouseLocal};
    }

    bool Window::notifySystemWindowReshape(int newScreenWidth, int newScreenHeight) {
        SystemWindowEventListener::notifySystemWindowReshape(newScreenWidth, newScreenHeight);

        this->notifyReshape(newScreenWidth, newScreenHeight);

        return true;
    }

    void Window::notifyReshape(int w, int h) { this->setSize(w, h); }

    void Window::setDecorate(bool _decorate) { this->decorate = _decorate; }

    void Window::setClear(bool _clear) { this->clear = _clear; }

    bool Window::notifyRender() {
        this->draw();

        return SystemWindowEventListener::notifyRender();
    }

    RectI Window::getEffectiveViewport() const {
        int xtraPadding = flags & HasMainMenu ? Graphics::menuHeight : 0;

        auto _x = getx() + Graphics::hPadding,
                _y = gety() + Graphics::vPadding,
                _w = getw() - 2 * Graphics::hPadding,
                _h = geth() - 2 * Graphics::vPadding - xtraPadding;

        return {_x, _x + _w, _y, _y + _h};
    }

    void Window::setBGColor(Color color) { backgroundColor = color; }
    auto Window::getBGColor() const -> const Color & { return backgroundColor; }

    bool
    Window::notifyMouseButton(MouseButton button, KeyState state, ModKeys keys) {
        if     (button == MouseButton::MouseButton_LEFT)   mouseLeftButton   = state;
        else if(button == MouseButton::MouseButton_MIDDLE) mouseCenterButton = state;
        else if(button == MouseButton::MouseButton_RIGHT)  mouseRightButton  = state;

        return SystemWindowEventListener::notifyMouseButton(button, state, keys);
    }
}