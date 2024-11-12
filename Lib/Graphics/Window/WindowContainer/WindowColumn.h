//
// Created by joao on 20/08/23.
//

#ifndef STUDIOSLAB_WINDOWCOLUMN_H
#define STUDIOSLAB_WINDOWCOLUMN_H

#include <list>
#include "Graphics/Window/SlabWindow.h"



namespace Slab::Graphics {

    class WindowColumn : public SlabWindow {
        std::list<Pointer<SlabWindow>> windows;
        RealVector heights;

        bool assertConsistency() const;

    public:
        WindowColumn() = default;

        void addWindow(const Pointer<SlabWindow>& window, float windowHeight = -1);

        bool removeWindow(const Pointer<SlabWindow>&);

        bool isEmpty() const;

        void arrangeWindows();

        void draw() override;

        void notifyReshape(int newWinW, int newWinH) override;

        bool notifyKeyboard(KeyMap key, KeyState state, ModKeys modKeys) override;

        bool notifyMouseMotion(int x, int y, int dx, int dy) override;

        bool notifyMouseButton(MouseButton button, KeyState state,
                               ModKeys keys) override;

        bool notifyMouseWheel(double dx, double dy) override;

        auto setx(int x) -> void override;

        auto sety(int y) -> void override;
    };


}

#endif //STUDIOSLAB_WINDOWCOLUMN_H
