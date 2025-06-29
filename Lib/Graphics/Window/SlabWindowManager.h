//
// Created by joao on 10/19/24.
//

#ifndef STUDIOSLAB_SLABWINDOWMANAGER_H
#define STUDIOSLAB_SLABWINDOWMANAGER_H

#include "SlabWindow.h"
#include "Utils/List.h"
#include "Decorator.h"
#include "WindowManager.h"

namespace Slab::Graphics {

    class SlabWindowManager final : public FWindowManager {
        struct WindowMetaInformation {
            Pointer<FSlabWindow> window= nullptr;
            bool is_full_screen = false;
            bool is_hidden = false;

            bool operator<(const WindowMetaInformation &rhs) const;
            bool operator>(const WindowMetaInformation &rhs) const;
            bool operator<=(const WindowMetaInformation &rhs) const;
            bool operator>=(const WindowMetaInformation &rhs) const;
            bool operator==(const WindowMetaInformation &rhs) const;
            bool operator==(const std::nullptr_t &rhs) const;
        };

        List<Pointer<WindowMetaInformation>> slab_windows;

        Decorator decorator;

        Int w_system_window=10, h_system_window=10;

        Pointer<WindowMetaInformation> focused;
        using Anchor = Point2D;
        struct Grabbed {Anchor anchor; enum What {None, Titlebar, Corner} what; Pointer<FSlabWindow> window;} Grabbed;

    public:
        explicit SlabWindowManager(SystemWindow* Parent=nullptr);
        ~SlabWindowManager() override = default;

        void setFocus(const Pointer<WindowMetaInformation>&);

        void AddSlabWindow(const Pointer<FSlabWindow>&, bool hidden) override;

        bool NotifyKeyboard(KeyMap key, KeyState state, ModKeys modKeys) override;

        bool NotifyMouseButton(MouseButton button, KeyState state, ModKeys keys) override;

        bool notifyMouseMotion(int x, int y, int dx, int dy) override;

        bool notifyMouseWheel(double dx, double dy) override;

        bool notifyFilesDropped(StrVector paths) override;

        bool notifySystemWindowReshape(int w, int h) override;

        bool notifyRender() override;

    };

} // Slab::Graphics

#endif //STUDIOSLAB_SLABWINDOWMANAGER_H
