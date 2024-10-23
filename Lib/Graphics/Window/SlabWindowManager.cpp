//
// Created by joao on 10/19/24.
//

#include "SlabWindowManager.h"
#include "Decorator.h"
#include "WindowStyles.h"
#include "Graphics/SlabGraphics.h"
#include <ranges>


namespace Slab::Graphics {

    void SlabWindowManager::addSlabWindow(const Pointer<SlabWindow>& slab_window) {
        slab_windows.push_back(slab_window);

        if(focused == nullptr) setFocus(slab_window);
    }

    bool SlabWindowManager::notifyKeyboard(KeyMap key, KeyState state, ModKeys modKeys) {
        if(focused == nullptr) return false;

        return focused->notifyKeyboard(key, state, modKeys);
    }

    bool SlabWindowManager::notifyMouseButton(MouseButton button, KeyState state, ModKeys keys) {

        if(button==MouseButton_LEFT && state==Press) {
            auto mouse_state = Graphics::GetGraphicsBackend().getMouseState();

            auto first = FindFirst_If(slab_windows, [mouse_state, this](const Pointer<SlabWindow> &window) {
                fix is_mouse_in = window->isMouseIn();
                fix is_decorated = !(window->getFlags() & SlabWindow::NoDecoration);
                fix is_mouse_over_grab_region = decorator.isMouseOverGrabRegion(*window, mouse_state.x, mouse_state.y);

                return is_mouse_in || (is_mouse_over_grab_region && is_decorated);
            });

            if(first != slab_windows.end()) {
                setFocus(*first);

                if (decorator.isMouseOverTitlebar(*focused, mouse_state.x, mouse_state.y)) {
                    grabbed = {Point2D(mouse_state.x - focused->getx(), mouse_state.y - focused->gety()),
                               Grabbed::Titlebar,
                               focused};
                } else if(decorator.isMouseOverGrabRegion(*focused, mouse_state.x, mouse_state.y) && !focused->wantsFullscreen()) {
                    grabbed = {Point2D(focused->getw()+focused->getx() - mouse_state.x,
                                       focused->geth()+focused->gety() - mouse_state.y),
                               Grabbed::Corner,
                               focused};
                } else {
                    grabbed = {Point2D(), Grabbed::None, nullptr};
                }
            }
        } else if(state==Release) grabbed = {Point2D(), Grabbed::None, nullptr};

        if(focused == nullptr) return false;
        return focused->notifyMouseButton(button, state, keys);
    }

    bool SlabWindowManager::notifyMouseMotion(int x, int y, int dx, int dy) {
        if(grabbed.window != nullptr) {
            auto p = grabbed.anchor;

            if(grabbed.what == Grabbed::Titlebar) {
                fix x_min = -grabbed.window->getw() + 200;
                fix y_min = -menu_height+title_bar_height-tiling_gap;

                fix x_max = w_system_window-200;
                fix y_max = h_system_window-200;

                fix x_new = Min(Max(x - (int) p.x, x_min), (int)x_max);
                fix y_new = Min(Max(y - (int) p.y, y_min), (int)y_max);

                grabbed.window->setx(x_new);
                grabbed.window->sety(y_new);

            } else if(grabbed.what == Grabbed::Corner) {
                fix w = Max(x-grabbed.window->getx()+grabbed.anchor.x, 500.);
                fix h = Max(y-grabbed.window->gety()+grabbed.anchor.y, 230.);

                grabbed.window->notifyReshape((int)w, (int)h);
            }

            return true;
        }

        if(focused == nullptr) return false;

        return focused->notifyMouseMotion(x, y);
    }

    bool SlabWindowManager::notifyMouseWheel(double dx, double dy) {
        if(focused == nullptr) return false;

        return focused->notifyMouseWheel(dx, dy);
    }

    bool SlabWindowManager::notifyFilesDropped(StrVector paths) {
        return false;
    }

    bool SlabWindowManager::notifySystemWindowReshape(int w, int h) {
        w_system_window = w;
        h_system_window = h;

        decorator.setSystemWindowShape(w, h);

        for(auto &slab_window : slab_windows) {
            if (slab_window->wantsFullscreen() || slab_windows.size()==1) {
                slab_window->setDecorate(false);
                slab_window->setx(0);
                slab_window->sety(Graphics::menu_height);
                slab_window->notifyReshape(w, h - Graphics::menu_height);
            }
        }

        return false;
    }

    bool SlabWindowManager::notifyRender() {
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