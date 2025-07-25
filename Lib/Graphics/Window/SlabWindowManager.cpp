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
    Fix AnimationTimeInSeconds = 0.25;

    SlabWindowManager::SlabWindowManager()
    : FWindowManager()
    , Grabbed()
    , MouseState(New<FMouseState>())
    {
        AddResponder(MouseState);
    }

    void SlabWindowManager::AddSlabWindow(const TPointer<FSlabWindow>& slab_window, bool hidden) {
        const auto MetaWinData = New<WindowMetaInformation>(slab_window, false, hidden);

        SlabWindows.push_back(MetaWinData);

        if(CurrentlyFocused == nullptr) SetFocus(MetaWinData);
    }

    bool SlabWindowManager::NotifyKeyboard(EKeyMap key, EKeyState state, EModKeys modKeys) {
        if(CurrentlyFocused == nullptr) return false;

        if(key==Key_F11 && state==Release)
        {
            if(CurrentlyFocused->is_full_screen) {
                fix w = WindowStyle::DefaultWindowRect.GetWidth();
                fix h = WindowStyle::DefaultWindowRect.GetHeight();
                fix x = WindowStyle::DefaultWindowRect.xMin;
                fix y = WindowStyle::DefaultWindowRect.yMin;

                RectI rect_f = {x, x+w, y, y+h};
                RectI rect_0 = CurrentlyFocused->Window->Config.WinRect;
                RectI Δrect = rect_f+rect_0*(-1.0);

                // auto &rect = focused.window->config.win_rect;
                // Animator::Set(rect.xMin, x, anim_time);
                // Animator::Set(rect.yMin, y, anim_time);
                // Animator::Set(rect.xMax, x+w, anim_time);
                // Animator::Set(rect.yMax, y+h, anim_time);

                auto &window = CurrentlyFocused->Window;
                constexpr Int N=10000;
                Animator::SetCallback(0, N, AnimationTimeInSeconds,
                                      [&window, rect_0, Δrect](Int i) {
                                          fix s = (DevFloat(i)/N);
                                           fix rect = rect_0 + Δrect*s ;

                                          window->Set_x(rect.xMin);
                                          window->Set_y(rect.yMin);
                                          window->NotifyReshape(rect.GetWidth(), rect.GetHeight());
                                      });

                CurrentlyFocused->Window->SetDecorate(true);
                CurrentlyFocused->is_full_screen = false;
            } else  {
                fix w = w_system_window;
                fix h = h_system_window - WindowStyle::menu_height;
                fix x = 0;
                fix y = WindowStyle::menu_height;

                RectI rect_f = {x, x+w, y, y+h};
                RectI rect_0 = CurrentlyFocused->Window->Config.WinRect;
                RectI Δrect = rect_f+rect_0*(-1.0);

                // Animator::Set(rect.xMin, x, anim_time);
                // Animator::Set(rect.yMin, y, anim_time);
                // Animator::Set(rect.xMax, x+w, anim_time);
                // Animator::Set(rect.yMax, y+h, anim_time);

                auto &window = CurrentlyFocused->Window;
                constexpr Int N=10000;

                Animator::SetCallback(0, N, AnimationTimeInSeconds,
                    [&window, rect_0, Δrect](Int i) {
                        fix s = (DevFloat(i)/N);
                        fix rect = rect_0 + Δrect*s ;

                        window->Set_x(rect.xMin);
                        window->Set_y(rect.yMin);
                        window->NotifyReshape(rect.GetWidth(), rect.GetHeight());
                    },
                    [window](){
                        window->SetDecorate(false);
                });

                CurrentlyFocused->is_full_screen = true;
            }

        }

        return CurrentlyFocused->Window->NotifyKeyboard(key, state, modKeys);
    }

    bool SlabWindowManager::NotifyMouseButton(EMouseButton button, EKeyState state, EModKeys keys) {
        FWindowManager::NotifyMouseButton(button, state, keys); // Update delegates

        if(state==Press) {
            const auto First = FindFirst_If(SlabWindows, [this](const TPointer<WindowMetaInformation> &meta) {
                fix is_mouse_in = meta->Window->IsMouseInside();
                fix is_decorated = !(meta->Window->GetFlags() & SlabWindowNoDecoration);
                fix is_mouse_over_grab_region = Decorator.isMouseOverGrabRegion(*meta->Window,
                                                                                MouseState->x,
                                                                                MouseState->y);

                return is_mouse_in || (is_mouse_over_grab_region && is_decorated);
            });

            if(First != SlabWindows.end()) {
                SetFocus(*First);

                if (Decorator.isMouseOverTitlebar(*CurrentlyFocused->Window, MouseState->x, MouseState->y)) {
                    Grabbed = {Point2D(MouseState->x - CurrentlyFocused->Window->Get_x(), MouseState->y - CurrentlyFocused->Window->Get_y()),
                               Grabbed::Titlebar,
                               CurrentlyFocused->Window};
                } else if(Decorator.isMouseOverGrabRegion(*CurrentlyFocused->Window, MouseState->x, MouseState->y)
                      && !CurrentlyFocused->Window->WantsFullscreen()) {
                    Grabbed = {Point2D(CurrentlyFocused->Window->GetWidth() + CurrentlyFocused->Window->Get_x() - MouseState->x,
                                       CurrentlyFocused->Window->GetHeight() + CurrentlyFocused->Window->Get_y() - MouseState->y),
                               Grabbed::Corner,
                               CurrentlyFocused->Window};
                } else {
                    Grabbed = {Point2D(), Grabbed::None, nullptr};
                }
            }
        } else if(state==Release) Grabbed = {Point2D(), Grabbed::None, nullptr};

        if(CurrentlyFocused == nullptr) return false;
        return CurrentlyFocused->Window->NotifyMouseButton(button, state, keys);
    }

    bool SlabWindowManager::NotifyMouseMotion(int x, int y, int dx, int dy) {
        if(Grabbed.window != nullptr) {
            auto p = Grabbed.anchor;

            if(Grabbed.what == Grabbed::Titlebar) {
                fix x_min = -Grabbed.window->GetWidth() + 200;
                fix y_min = Decorator.titlebar_height() + WindowStyle::menu_height;

                fix x_max = w_system_window-200;
                fix y_max = h_system_window-200;

                fix x_new = Min(Max(x - (int) p.x, x_min), (int)x_max);
                fix y_new = Min(Max(y - (int) p.y, y_min), (int)y_max);

                Grabbed.window->Set_x(x_new);
                Grabbed.window->Set_y(y_new);

            } else if(Grabbed.what == Grabbed::Corner) {
                auto min_width = Grabbed.window->MinWidth;
                auto min_height = Grabbed.window->MinHeight;

                fix w = Max(x-Grabbed.window->Get_x()+Grabbed.anchor.x, (DevFloat)min_width);
                fix h = Max(y-Grabbed.window->Get_y()+Grabbed.anchor.y, (DevFloat)min_height);

                Grabbed.window->NotifyReshape((int)w, (int)h);
            }

            return true;
        }

        if(CurrentlyFocused == nullptr) return false;

        return CurrentlyFocused->Window->NotifyMouseMotion(x, y, dx, dy);
    }

    bool SlabWindowManager::NotifyMouseWheel(double dx, double dy) {
        if(CurrentlyFocused == nullptr) return false;

        return CurrentlyFocused->Window->NotifyMouseWheel(dx, dy);
    }

    bool SlabWindowManager::NotifyFilesDropped(StrVector paths) {
        return false;
    }

    bool SlabWindowManager::NotifySystemWindowReshape(int w, int h) {
        w_system_window = w;
        h_system_window = h;

        Decorator.setSystemWindowShape(w, h);

        for(auto &meta : SlabWindows) {
            auto slab_window = meta->Window;
            if (meta->is_full_screen || slab_window->WantsFullscreen() || SlabWindows.size()==1) {
                meta->is_full_screen = true;
                slab_window->SetDecorate(false);
                slab_window->Set_x(0);
                slab_window->Set_y(WindowStyle::menu_height);
                slab_window->NotifyReshape(w, h - WindowStyle::menu_height);
            }
        }

        return false;
    }

    bool SlabWindowManager::NotifyRender(const FPlatformWindow& PlatformWindow) {
        for (IN MetaSlabWindow : std::ranges::reverse_view(SlabWindows)) {
            Decorator.begin_decoration(*MetaSlabWindow->Window, MouseState->x, MouseState->y);
            MetaSlabWindow->Window->ImmediateDraw(PlatformWindow);
            Decorator.FinishDecoration(*MetaSlabWindow->Window, MouseState->x, MouseState->y);
        }

        return true;
    }

    void SlabWindowManager::SetFocus(const TPointer<WindowMetaInformation>& meta)
    {
        if(CurrentlyFocused != nullptr) CurrentlyFocused->Window->NotifyBecameInactive();

        CurrentlyFocused = meta;
        meta->Window->NotifyBecameActive();
        meta->is_hidden = false;

        if(!Contains(SlabWindows, meta)) SlabWindows.emplace_front(meta);
        else MoveToFront(SlabWindows, CurrentlyFocused);
    }

    bool
    SlabWindowManager::WindowMetaInformation::operator<(const WindowMetaInformation &rhs) const {
        return Window < rhs.Window;
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
        return Window==rhs.Window;
    }

    bool SlabWindowManager::WindowMetaInformation::operator==(std::
        nullptr_t const &rhs) const {
        return Window==rhs;
    }
} // Slab::Graphics