//
// Created by joao on 24/05/24.
//

#ifndef STUDIOSLAB_WINDOWCONTAINER_H
#define STUDIOSLAB_WINDOWCONTAINER_H

#include "Graphics/Window/Window.h"

#include <list>

namespace Slab::Graphics {

    class WindowContainer : public Window {
        std::list<Window_ptr> windows;

    public:
        WindowContainer() = default;

        bool removeWindow(const Window_ptr&);
        bool isEmpty() const;

        void draw() override;

        virtual void arrangeWindows() = 0;

        void notifyReshape(int newWinW, int newWinH) override;
        bool notifyKeyboard(KeyMap key, KeyState state, ModKeys modKeys) override;
        bool notifyFilesDropped(StrVector paths) override;
        bool notifyMouseMotion(int x, int y) override;
        bool notifyMouseButton(MouseButton button, KeyState state,
                               ModKeys keys) override;
        bool notifyMouseWheel(double dx, double dy) override;
    };

} // Slab::Graphics

#endif //STUDIOSLAB_WINDOWCONTAINER_H
