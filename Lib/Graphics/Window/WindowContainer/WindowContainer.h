//
// Created by joao on 24/05/24.
//

#ifndef STUDIOSLAB_WINDOWCONTAINER_H
#define STUDIOSLAB_WINDOWCONTAINER_H

#include "Graphics/Window/SlabWindow.h"

#include <list>

namespace Slab::Graphics {

    class WindowContainer : public SlabWindow {
        std::list<Pointer<SlabWindow>> windows;

    public:
        WindowContainer() = default;

        bool removeWindow(const Pointer<SlabWindow>&);
        bool isEmpty() const;

        void draw() override;

        virtual void arrangeWindows() = 0;

        void notifyReshape(int newWinW, int newWinH) override;
        bool notifyKeyboard(KeyMap key, KeyState state, ModKeys modKeys) override;
        bool notifyMouseMotion(int x, int y, int dx, int dy) override;
        bool notifyMouseButton(MouseButton button, KeyState state,
                               ModKeys keys) override;
        bool notifyMouseWheel(double dx, double dy) override;
    };

} // Slab::Graphics

#endif //STUDIOSLAB_WINDOWCONTAINER_H
