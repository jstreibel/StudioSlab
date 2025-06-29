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
    SlabWindowManager::SlabWindowManager(SystemWindow *Parent)
        : FWindowManager(Parent), Grabbed() {
    }

    void SlabWindowManager::AddSlabWindow(const Pointer<FSlabWindow>& slab_window, bool hidden) {
        auto meta = New<WindowMetaInformation>(slab_window, false, hidden);

        slab_windows.push_back(meta);

        if(focused == nullptr) setFocus(meta);
    }

    bool SlabWindowManager::NotifyKeyboard(KeyMap key, KeyState state, ModKeys modKeys) {
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
                                          fix s = (DevFloat(i)/N);
                                           fix rect = rect_0 + Δrect*s ;

                                          window->Set_x(rect.xMin);
                                          window->Set_y(rect.yMin);
                                          window->NotifyReshape(rect.width(), rect.height());
                                      });

                focused->window->SetDecorate(true);
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
                        fix s = (DevFloat(i)/N);
                        fix rect = rect_0 + Δrect*s ;

                        window->Set_x(rect.xMin);
                        window->Set_y(rect.yMin);
                        window->NotifyReshape(rect.width(), rect.height());
                    },
                    [window](){
                        window->SetDecorate(false);
                });

                focused->is_full_screen = true;
            }

        }

        return focused->window->NotifyKeyboard(key, state, modKeys);
    }

    bool SlabWindowManager::NotifyMouseButton(MouseButton button, KeyState state, ModKeys keys) {

        if(state==Press) {
            auto mouse_state = parent_system_window->GetMouseState();

            auto first = FindFirst_If(slab_windows, [mouse_state, this](const Pointer<WindowMetaInformation> &meta) {
                fix is_mouse_in = meta->window->isMouseIn();
                fix is_decorated = !(meta->window->getFlags() & SlabWindowNoDecoration);
                fix is_mouse_over_grab_region = decorator.isMouseOverGrabRegion(*meta->window,
                                                                                mouse_state->x,
                                                                                mouse_state->y);

                return is_mouse_in || (is_mouse_over_grab_region && is_decorated);
            });

            if(first != slab_windows.end()) {
                setFocus(*first);

                if (decorator.isMouseOverTitlebar(*focused->window, mouse_state->x, mouse_state->y)) {
                    Grabbed = {Point2D(mouse_state->x - focused->window->getx(), mouse_state->y - focused->window->gety()),
                               Grabbed::Titlebar,
                               focused->window};
                } else if(decorator.isMouseOverGrabRegion(*focused->window, mouse_state->x, mouse_state->y)
                      && !focused->window->wantsFullscreen()) {
                    Grabbed = {Point2D(focused->window->GetWidth() + focused->window->getx() - mouse_state->x,
                                       focused->window->GetHeight() + focused->window->gety() - mouse_state->y),
                               Grabbed::Corner,
                               focused->window};
                } else {
                    Grabbed = {Point2D(), Grabbed::None, nullptr};
                }
            }
        } else if(state==Release) Grabbed = {Point2D(), Grabbed::None, nullptr};

        if(focused == nullptr) return false;
        return focused->window->NotifyMouseButton(button, state, keys);
    }

    bool SlabWindowManager::NotifyMouseMotion(int x, int y, int dx, int dy) {
        if(Grabbed.window != nullptr) {
            auto p = Grabbed.anchor;

            if(Grabbed.what == Grabbed::Titlebar) {
                fix x_min = -Grabbed.window->GetWidth() + 200;
                fix y_min = decorator.titlebar_height() + WindowStyle::menu_height;

                fix x_max = w_system_window-200;
                fix y_max = h_system_window-200;

                fix x_new = Min(Max(x - (int) p.x, x_min), (int)x_max);
                fix y_new = Min(Max(y - (int) p.y, y_min), (int)y_max);

                Grabbed.window->Set_x(x_new);
                Grabbed.window->Set_y(y_new);

            } else if(Grabbed.what == Grabbed::Corner) {
                auto min_width = Grabbed.window->min_width;
                auto min_height = Grabbed.window->min_height;

                fix w = Max(x-Grabbed.window->getx()+Grabbed.anchor.x, (DevFloat)min_width);
                fix h = Max(y-Grabbed.window->gety()+Grabbed.anchor.y, (DevFloat)min_height);

                Grabbed.window->NotifyReshape((int)w, (int)h);
            }

            return true;
        }

        if(focused == nullptr) return false;

        return focused->window->NotifyMouseMotion(x, y, dx, dy);
    }

    bool SlabWindowManager::NotifyMouseWheel(double dx, double dy) {
        if(focused == nullptr) return false;

        return focused->window->NotifyMouseWheel(dx, dy);
    }

    bool SlabWindowManager::NotifyFilesDropped(StrVector paths) {
        return false;
    }

    bool SlabWindowManager::NotifySystemWindowReshape(int w, int h) {
        w_system_window = w;
        h_system_window = h;

        decorator.setSystemWindowShape(w, h);

        for(auto &meta : slab_windows) {
            auto slab_window = meta->window;
            if (meta->is_full_screen || slab_window->wantsFullscreen() || slab_windows.size()==1) {
                meta->is_full_screen = true;
                slab_window->SetDecorate(false);
                slab_window->Set_x(0);
                slab_window->Set_y(WindowStyle::menu_height);
                slab_window->NotifyReshape(w, h - WindowStyle::menu_height);
            }
        }

        return false;
    }

    bool SlabWindowManager::NotifyRender() {
        for (auto & slab_window : std::ranges::reverse_view(slab_windows)) {
            auto mouse = parent_system_window->GetMouseState();
            decorator.begin_decoration(*slab_window->window, mouse->x, mouse->y);
            slab_window->window->Draw();
            decorator.finish_decoration(*slab_window->window, mouse->x, mouse->y);
        }

        return true;
    }

    void SlabWindowManager::setFocus(const Pointer<WindowMetaInformation>& meta)
    {
        if(focused != nullptr) focused->window->notifyBecameInactive();

        focused = meta;
        meta->window->notifyBecameActive();
        meta->is_hidden = false;

        if(!Contains(slab_windows, meta)) slab_windows.emplace_front(meta);
        else MoveToFront(slab_windows, focused);
    }

    bool
    SlabWindowManager::WindowMetaInformation::operator<(const WindowMetaInformation &rhs) const {
        return window < rhs.window;
    }

    bool
    SlabWindowManager::WindowMetaInformation::operator>(const WindowMetaInformation &rhs) const {
        return rhs < *this;
    }

    bool
    SlabWindowManager::WindowMetaInformation::operator<=(const WindowMetaInformation &rhs) const {
        return !(rhs < *this);
    }

    bool
    SlabWindowManager::WindowMetaInformation::operator>=(const WindowMetaInformation &rhs) const {
        return !(*this < rhs);
    }

    bool
    SlabWindowManager::WindowMetaInformation::operator==(const WindowMetaInformation &rhs) const {
        return window==rhs.window;
    }

    bool SlabWindowManager::WindowMetaInformation::operator==(std::
        nullptr_t const &rhs) const {
        return window==rhs;
    }
} // Slab::Graphics