//
// Created by joao on 10/19/24.
//

#ifndef STUDIOSLAB_WINDOWMANAGER_H
#define STUDIOSLAB_WINDOWMANAGER_H

#include "Graphics/Backend/Events/SystemWindowEventListener.h"
#include "Window.h"

namespace Slab::Graphics {

    class WindowManager : public SystemWindowEventListener {
        List<Pointer<Window>> slab_windows;

        Pointer<Window> focused;

    public:
        ~WindowManager() override = default;

        void addSlabWindow(Pointer<Window>);

        bool notifyKeyboard(KeyMap key, KeyState state, ModKeys modKeys) override;

        bool notifyMouseButton(MouseButton button, KeyState state, ModKeys keys) override;

        bool notifyMouseMotion(int x, int y) override;

        bool notifyMouseWheel(double dx, double dy) override;

        bool notifyFilesDropped(StrVector paths) override;

        bool notifySystemWindowReshape(int w, int h) override;

        bool notifyRender() override;
    };

} // Slab::Graphics

#endif //STUDIOSLAB_WINDOWMANAGER_H
