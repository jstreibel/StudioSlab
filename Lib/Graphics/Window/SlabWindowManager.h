//
// Created by joao on 10/19/24.
//

#ifndef STUDIOSLAB_SLAB_WINDOW_MANAGER_H
#define STUDIOSLAB_SLAB_WINDOW_MANAGER_H

#include "SlabWindow.h"
#include "Utils/List.h"
#include "Decorator.h"
#include "WindowManager.h"

namespace Slab::Graphics {

    class SlabWindowManager final : public FWindowManager {
        struct WindowMetaInformation {
            TPointer<FSlabWindow> Window= nullptr;
            bool is_full_screen = false;
            bool is_hidden = false;

            bool operator<(const WindowMetaInformation &rhs) const;
            bool operator>(const WindowMetaInformation &rhs) const;
            bool operator<=(const WindowMetaInformation &rhs) const;
            bool operator>=(const WindowMetaInformation &rhs) const;
            bool operator==(const WindowMetaInformation &rhs) const;
            bool operator==(const std::nullptr_t &rhs) const;
        };

        TList<TPointer<WindowMetaInformation>> SlabWindows;

        FDecorator Decorator;

        Int WidthSysWin=10, HeightSysWin=10;

        TPointer<WindowMetaInformation> CurrentlyFocused;
        using Anchor = Point2D;
        struct Grabbed {Anchor anchor; enum What {None, Titlebar, Corner} what; TPointer<FSlabWindow> window;} Grabbed;

        TPointer<FMouseState> MouseState;

    public:
        explicit SlabWindowManager();
        ~SlabWindowManager() override = default;

        void SetFocus(const TPointer<WindowMetaInformation>&);

        void AddSlabWindow(const TPointer<FSlabWindow>&, bool hidden) override;

        bool NotifyKeyboard(EKeyMap key, EKeyState state, EModKeys modKeys) override;

        bool NotifyMouseButton(EMouseButton button, EKeyState state, EModKeys keys) override;

        bool NotifyMouseMotion(int x, int y, int dx, int dy) override;

        bool NotifyMouseWheel(double dx, double dy) override;

        bool NotifyFilesDropped(StrVector paths) override;

        bool NotifySystemWindowReshape(int w, int h) override;

        bool NotifyRender(const FPlatformWindow&) override;

    };

} // Slab::Graphics

#endif //STUDIOSLAB_SLAB_WINDOW_MANAGER_H
