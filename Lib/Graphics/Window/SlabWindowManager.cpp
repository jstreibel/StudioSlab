//
// Created by joao on 10/19/24.
//

#include "SlabWindowManager.h"
#include "Decorator.h"
#include "WindowStyles.h"
#include "Graphics/SlabGraphics.h"
#include <functional>
#include <ranges>


namespace Slab::Graphics {

    void SlabWindowManager::addSlabWindow(Pointer<SlabWindow> slab_window) {
        slab_windows.push_back(slab_window);

        if(focused == nullptr) setFocus(slab_window);
    }

    bool SlabWindowManager::notifyKeyboard(KeyMap key, KeyState state, ModKeys modKeys) {
        if(focused == nullptr) return false;

        return focused->notifyKeyboard(key, state, modKeys);
    }

    bool SlabWindowManager::notifyMouseButton(MouseButton button, KeyState state, ModKeys keys) {
        if(focused == nullptr) return false;

        if(button==MouseButton_LEFT && state==Press) {
            auto mouse_state = Graphics::GetGraphicsBackend().getMouseState();

            auto first = FindFirst_If(slab_windows, [mouse_state, this](const Pointer<SlabWindow> &window) {
                return window->isMouseIn() || decorator.isMouseOverGrabRegion(*window, mouse_state.x, mouse_state.y);
            });

            if(first != slab_windows.end()) {
                setFocus(*first);

                if (decorator.isMouseOverTitlebar(*focused, mouse_state.x, mouse_state.y)) {
                    grabbed = {Point2D(mouse_state.x - focused->getx(), mouse_state.y - focused->gety()),
                               Grabbed::Titlebar,
                               focused};
                } else if(decorator.isMouseOverGrabRegion(*focused, mouse_state.x, mouse_state.y)) {
                    grabbed = {Point2D(focused->getw()+focused->getx() - mouse_state.x,
                                       focused->geth()+focused->gety() - mouse_state.y),
                               Grabbed::Corner,
                               focused};
                } else {
                    grabbed = {Point2D(), Grabbed::None, nullptr};
                }
            }
        } else if(state==Release) grabbed = {Point2D(), Grabbed::None, nullptr};

        return focused->notifyMouseButton(button, state, keys);
    }

    bool SlabWindowManager::notifyMouseMotion(int x, int y, int dx, int dy) {
        if(focused == nullptr) return false;

        if(grabbed.window != nullptr) {
            auto p = grabbed.anchor;

            if(grabbed.what == Grabbed::Titlebar) {
                grabbed.window->setx(x - (int) p.x);
                grabbed.window->sety(y - (int) p.y);
            } else if(grabbed.what == Grabbed::Corner) {
                fix w = x-grabbed.window->getx()+grabbed.anchor.x;
                fix h = y-grabbed.window->gety()+grabbed.anchor.y;
                grabbed.window->notifyReshape((int)w, (int)h);
            }

            return true;
        }

        return focused->notifyMouseMotion(x, y);
    }

    bool SlabWindowManager::notifyMouseWheel(double dx, double dy) {
        if(focused == nullptr) return false;

        return focused->notifyMouseWheel(dx, dy);
    }

    bool SlabWindowManager::notifyFilesDropped(StrVector paths) {
        if(focused == nullptr) return false;

        return false;
    }

    bool SlabWindowManager::notifySystemWindowReshape(int w, int h) {
        decorator.setSystemWindowShape(w, h);

        for(auto &slab_window : slab_windows) {
            slab_window->setupParentSystemWindowHeight(h);

            if (slab_window->wantsFullscreen() || slab_windows.size()==1) {
                slab_window->setDecorate(false);
                slab_window->setx(0);
                slab_window->sety(Graphics::menuHeight);
                slab_window->notifyReshape(w, h - Graphics::menuHeight);
            }
        }

        return false;
    }

    bool SlabWindowManager::notifyRender() {
        if(focused == nullptr) return false;

        for (auto & slab_window : std::ranges::reverse_view(slab_windows)) {
            auto mouse = GetGraphicsBackend().getMouseState();
            decorator.decorate(*slab_window, mouse.x, mouse.y);
            slab_window->draw();
        }

        return true;
    }

    void SlabWindowManager::setFocus(const Pointer<SlabWindow>& win) {
        if(focused != nullptr) focused->notifyBecameInactive() ;

        focused = win;
        win->notifyBecameActive();

        if(!Contains(slab_windows, win)) slab_windows.emplace_front(win);
        else MoveToFront(slab_windows, focused);
    }


} // Slab::Graphics