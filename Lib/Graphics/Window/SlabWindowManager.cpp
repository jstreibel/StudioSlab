//
// Created by joao on 10/19/24.
//

#include "SlabWindowManager.h"
#include "Decorator.h"
#include "WindowStyles.h"
#include "Graphics/SlabGraphics.h"

#include "Core/SlabCore.h"
#include "Graphics/Modules/Animator/Animator.h"

#include <ranges>
#include <utility>


namespace Slab::Graphics {
    void SlabWindowManager::addSlabWindow(const Pointer<SlabWindow>& slab_window, bool hidden) {
        auto meta = New<WindowMetaInformation>(slab_window, false, hidden);

        slab_windows.push_back(meta);

        if(focused == nullptr) setFocus(meta);
    }

    bool SlabWindowManager::notifyKeyboard(KeyMap key, KeyState state, ModKeys modKeys) {
        if(focused == nullptr) return false;

        fix anim_time = 0.25;

        if(key==Key_F11 && state==Release){
            if(focused->is_full_screen) {
                fix w = WindowStyle::default_window_rect.width();
                fix h = WindowStyle::default_window_rect.height();
                fix x = WindowStyle::default_window_rect.xMin;
                fix y = WindowStyle::default_window_rect.yMin;

                RectI rect_f = {x, x+w, y, y+h};
                RectI rect_0 = focused->window->config.win_rect;
                RectI Δrect = rect_f+rect_0*(-1.0);

                // auto &rect = focused.window->config.win_rect;
                // Animator::Set(rect.xMin, x, anim_time);
                // Animator::Set(rect.yMin, y, anim_time);
                // Animator::Set(rect.xMax, x+w, anim_time);
                // Animator::Set(rect.yMax, y+h, anim_time);

                auto &window = focused->window;
                constexpr Int N=10000;
                Animator::SetCallback(0, N, anim_time,
                                      [&window, rect_0, Δrect](Int i) {
                                          fix s = (Real(i)/N);
                                           fix rect = rect_0 + Δrect*s ;

                                          window->setx(rect.xMin);
                                          window->sety(rect.yMin);
                                          window->notifyReshape(rect.width(), rect.height());
                                      });

                focused->window->setDecorate(true);
                focused->is_full_screen = false;
            } else  {
                fix w = w_system_window;
                fix h = h_system_window - WindowStyle::menu_height;
                fix x = 0;
                fix y = WindowStyle::menu_height;

                RectI rect_f = {x, x+w, y, y+h};
                RectI rect_0 = focused->window->config.win_rect;
                RectI Δrect = rect_f+rect_0*(-1.0);

                // Animator::Set(rect.xMin, x, anim_time);
                // Animator::Set(rect.yMin, y, anim_time);
                // Animator::Set(rect.xMax, x+w, anim_time);
                // Animator::Set(rect.yMax, y+h, anim_time);

                auto &window = focused->window;
                constexpr Int N=10000;

                Animator::SetCallback(0, N, anim_time,
                    [&window, rect_0, Δrect](Int i) {
                        fix s = (Real(i)/N);
                        fix rect = rect_0 + Δrect*s ;

                        window->setx(rect.xMin);
                        window->sety(rect.yMin);
                        window->notifyReshape(rect.width(), rect.height());
                    },
                    [window](){
                        window->setDecorate(false);
                });

                focused->is_full_screen = true;
            }

        }

        return focused->window->notifyKeyboard(key, state, modKeys);
    }

    bool SlabWindowManager::notifyMouseButton(MouseButton button, KeyState state, ModKeys keys) {

        if(button==MouseButton_LEFT && state==Press) {
            auto mouse_state = parent_system_window->getMouseState();

            auto first = FindFirst_If(slab_windows, [mouse_state, this](const Pointer<WindowMetaInformation> &meta) {
                fix is_mouse_in = meta->window->isMouseIn();
                fix is_decorated = !(meta->window->getFlags() & SlabWindow::NoDecoration);
                fix is_mouse_over_grab_region = decorator.isMouseOverGrabRegion(*meta->window,
                                                                                mouse_state->x,
                                                                                mouse_state->y);

                return is_mouse_in || (is_mouse_over_grab_region && is_decorated);
            });

            if(first != slab_windows.end()) {
                setFocus(*first);

                if (decorator.isMouseOverTitlebar(*focused->window, mouse_state->x, mouse_state->y)) {
                    grabbed = {Point2D(mouse_state->x - focused->window->getx(), mouse_state->y - focused->window->gety()),
                               Grabbed::Titlebar,
                               focused->window};
                } else if(decorator.isMouseOverGrabRegion(*focused->window, mouse_state->x, mouse_state->y)
                      && !focused->window->wantsFullscreen()) {
                    grabbed = {Point2D(focused->window->GetWidth() + focused->window->getx() - mouse_state->x,
                                       focused->window->GetHeight() + focused->window->gety() - mouse_state->y),
                               Grabbed::Corner,
                               focused->window};
                } else {
                    grabbed = {Point2D(), Grabbed::None, nullptr};
                }
            }
        } else if(state==Release) grabbed = {Point2D(), Grabbed::None, nullptr};

        if(focused == nullptr) return false;
        return focused->window->notifyMouseButton(button, state, keys);
    }

    bool SlabWindowManager::notifyMouseMotion(int x, int y, int dx, int dy) {
        if(grabbed.window != nullptr) {
            auto p = grabbed.anchor;

            if(grabbed.what == Grabbed::Titlebar) {
                fix x_min = -grabbed.window->GetWidth() + 200;
                fix y_min = decorator.titlebar_height() + WindowStyle::menu_height;

                fix x_max = w_system_window-200;
                fix y_max = h_system_window-200;

                fix x_new = Min(Max(x - (int) p.x, x_min), (int)x_max);
                fix y_new = Min(Max(y - (int) p.y, y_min), (int)y_max);

                grabbed.window->setx(x_new);
                grabbed.window->sety(y_new);

            } else if(grabbed.what == Grabbed::Corner) {
                auto min_width = grabbed.window->min_width;
                auto min_height = grabbed.window->min_height;

                fix w = Max(x-grabbed.window->getx()+grabbed.anchor.x, (Real)min_width);
                fix h = Max(y-grabbed.window->gety()+grabbed.anchor.y, (Real)min_height);

                grabbed.window->notifyReshape((int)w, (int)h);
            }

            return true;
        }

        if(focused == nullptr) return false;

        return focused->window->notifyMouseMotion(x, y, dx, dy);
    }

    bool SlabWindowManager::notifyMouseWheel(double dx, double dy) {
        if(focused == nullptr) return false;

        return focused->window->notifyMouseWheel(dx, dy);
    }

    bool SlabWindowManager::notifyFilesDropped(StrVector paths) {
        return false;
    }

    bool SlabWindowManager::notifySystemWindowReshape(int w, int h) {
        w_system_window = w;
        h_system_window = h;

        decorator.setSystemWindowShape(w, h);

        for(auto &meta : slab_windows) {
            auto slab_window = meta->window;
            if (meta->is_full_screen || slab_window->wantsFullscreen() || slab_windows.size()==1) {
                meta->is_full_screen = true;
                slab_window->setDecorate(false);
                slab_window->setx(0);
                slab_window->sety(WindowStyle::menu_height);
                slab_window->notifyReshape(w, h - WindowStyle::menu_height);
            }
        }

        return false;
    }

    bool SlabWindowManager::notifyRender() {
        for (auto & slab_window : std::ranges::reverse_view(slab_windows)) {
            auto mouse = parent_system_window->getMouseState();
            decorator.begin_decoration(*slab_window->window, mouse->x, mouse->y);
            slab_window->window->draw();
            decorator.finish_decoration(*slab_window->window, mouse->x, mouse->y);
        }

        return true;
    }

    void SlabWindowManager::setFocus(Pointer<WindowMetaInformation> meta) {
        if(focused != nullptr) focused->window->notifyBecameInactive();

        focused = meta;
        meta->window->notifyBecameActive();
        meta->is_hidden = false;

        if(!Contains(slab_windows, meta)) slab_windows.emplace_front(meta);
        else MoveToFront(slab_windows, focused);
    }

    SlabWindowManager::SlabWindowManager(Pointer<SystemWindow> parent_syswin)
    : parent_system_window(parent_syswin==nullptr ?
                            GetGraphicsBackend()->GetMainSystemWindow() :
                            std::move(parent_syswin)) {    }

    bool
    SlabWindowManager::WindowMetaInformation::operator<(const SlabWindowManager::WindowMetaInformation &rhs) const {
        return window < rhs.window;
    }

    bool
    SlabWindowManager::WindowMetaInformation::operator>(const SlabWindowManager::WindowMetaInformation &rhs) const {
        return rhs < *this;
    }

    bool
    SlabWindowManager::WindowMetaInformation::operator<=(const SlabWindowManager::WindowMetaInformation &rhs) const {
        return !(rhs < *this);
    }

    bool
    SlabWindowManager::WindowMetaInformation::operator>=(const SlabWindowManager::WindowMetaInformation &rhs) const {
        return !(*this < rhs);
    }

    bool
    SlabWindowManager::WindowMetaInformation::operator==(const SlabWindowManager::WindowMetaInformation &rhs) const {
        return window==rhs.window;
    }

    bool SlabWindowManager::WindowMetaInformation::operator==(nullptr_t const &rhs) const {
        return window==rhs;
    }
} // Slab::Graphics