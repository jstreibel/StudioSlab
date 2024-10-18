//
// Created by joao on 7/9/24.
//

#ifndef STUDIOSLAB_WINDOWPROXY_H
#define STUDIOSLAB_WINDOWPROXY_H

#include "Window.h"

namespace Slab {
    namespace Graphics {

        class WindowProxy : public Window {
            Pointer<Window> window;

        public:
            WindowProxy();

            void draw() override;

            void setWindow(Pointer<Window>);

            void notifyReshape(int w, int h) override;

            bool notifyMouseButton(MouseButton button, KeyState state, ModKeys keys) override;

            bool notifyRender() override;

            bool notifyKeyboard(KeyMap key, KeyState state, ModKeys modKeys) override;

            bool notifyMouseMotion(int x, int y) override;

            bool notifyMouseWheel(double dx, double dy) override;

            bool notifyFilesDropped(StrVector paths) override;
        };

    } // Slab
} // Graphics

#endif //STUDIOSLAB_WINDOWPROXY_H
