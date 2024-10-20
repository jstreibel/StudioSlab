//
// Created by joao on 10/19/24.
//

#ifndef STUDIOSLAB_SLABWINDOWMANAGER_H
#define STUDIOSLAB_SLABWINDOWMANAGER_H

#include "Graphics/Backend/Events/SystemWindowEventListener.h"
#include "SlabWindow.h"
#include "Utils/List.h"

namespace Slab::Graphics {

    class SlabWindowManager : public SystemWindowEventListener {
        List<Pointer<SlabWindow>> slab_windows;

        Pointer<SlabWindow> focused;
        using Anchor = Point2D;
        struct {Anchor anchor; Pointer<SlabWindow> window;} grabbed;

    public:
        ~SlabWindowManager() override = default;

        void addSlabWindow(Pointer<SlabWindow>);

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
