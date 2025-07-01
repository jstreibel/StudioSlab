//
// Created by joao on 24/05/24.
//

#ifndef STUDIOSLAB_WINDOWCONTAINER_H
#define STUDIOSLAB_WINDOWCONTAINER_H

#include "Graphics/Window/SlabWindow.h"

#include <list>

namespace Slab::Graphics {

    class WindowContainer : public FSlabWindow {
        std::list<Pointer<FSlabWindow>> windows;

    public:
        WindowContainer() = default;

        bool removeWindow(const Pointer<FSlabWindow>&);
        bool isEmpty() const;

        void ImmediateDraw(const FPlatformWindow&) override;

        virtual void arrangeWindows() = 0;

        void NotifyReshape(int newWinW, int newWinH) override;
        bool NotifyKeyboard(EKeyMap key, EKeyState state, EModKeys modKeys) override;
        bool NotifyMouseMotion(int x, int y, int dx, int dy) override;
        bool NotifyMouseButton(EMouseButton button, EKeyState state,
                               EModKeys keys) override;
        bool NotifyMouseWheel(double dx, double dy) override;
    };

} // Slab::Graphics

#endif //STUDIOSLAB_WINDOWCONTAINER_H
