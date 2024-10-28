//
// Created by joao on 10/19/24.
//

#ifndef STUDIOSLAB_SLABWINDOWMANAGER_H
#define STUDIOSLAB_SLABWINDOWMANAGER_H

#include "Graphics/Backend/Events/SystemWindowEventListener.h"
#include "SlabWindow.h"
#include "Utils/List.h"
#include "Decorator.h"

namespace Slab::Graphics {

    class SlabWindowManager : public SystemWindowEventListener {
        struct WindowMetaInformation {
            Pointer<SlabWindow> window= nullptr;
            bool is_full_screen = false;
            bool is_hidden = false;

            bool operator<(const WindowMetaInformation &rhs) const;
            bool operator>(const WindowMetaInformation &rhs) const;
            bool operator<=(const WindowMetaInformation &rhs) const;
            bool operator>=(const WindowMetaInformation &rhs) const;
            bool operator==(const WindowMetaInformation &rhs) const;
            bool operator==(const nullptr_t &rhs) const;
        };

        List<Pointer<WindowMetaInformation>> slab_windows;

        Decorator decorator;

        Int w_system_window=10, h_system_window=10;

        Pointer<WindowMetaInformation> focused;
        using Anchor = Point2D;
        struct Grabbed {Anchor anchor; enum What {None, Titlebar, Corner} what; Pointer<SlabWindow> window;} grabbed;

    public:
        ~SlabWindowManager() override = default;

        void setFocus(Pointer<WindowMetaInformation>);

        void addSlabWindow(const Pointer<SlabWindow>&, bool hidden=false);

        bool notifyKeyboard(KeyMap key, KeyState state, ModKeys modKeys) override;

        bool notifyMouseButton(MouseButton button, KeyState state, ModKeys keys) override;

        bool notifyMouseMotion(int x, int y, int dx, int dy) override;

        bool notifyMouseWheel(double dx, double dy) override;

        bool notifyFilesDropped(StrVector paths) override;

        bool notifySystemWindowReshape(int w, int h) override;

        bool notifyRender() override;
    };

} // Slab::Graphics

#endif //STUDIOSLAB_SLABWINDOWMANAGER_H
