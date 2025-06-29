//
// Created by joao on 20/08/23.
//

#ifndef STUDIOSLAB_WINDOWCOLUMN_H
#define STUDIOSLAB_WINDOWCOLUMN_H

#include <list>
#include "Graphics/Window/SlabWindow.h"



namespace Slab::Graphics {

    class WindowColumn : public FSlabWindow {
        std::list<Pointer<FSlabWindow>> windows;
        RealVector heights;

        bool assertConsistency() const;

    public:
        WindowColumn() = default;

        void addWindow(const Pointer<FSlabWindow>& window, float windowHeight = -1);

        bool removeWindow(const Pointer<FSlabWindow>&);

        bool isEmpty() const;

        void arrangeWindows();

        void Draw() override;

        void NotifyReshape(int newWinW, int newWinH) override;

        bool NotifyKeyboard(KeyMap key, KeyState state, ModKeys modKeys) override;

        bool notifyMouseMotion(int x, int y, int dx, int dy) override;

        bool NotifyMouseButton(MouseButton button, KeyState state,
                               ModKeys keys) override;

        bool notifyMouseWheel(double dx, double dy) override;

        auto Set_x(int x) -> void override;

        auto Set_y(int y) -> void override;
    };


}

#endif //STUDIOSLAB_WINDOWCOLUMN_H
