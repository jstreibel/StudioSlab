//
// Created by joao on 20/08/23.
//

#ifndef STUDIOSLAB_WINDOWROW_H
#define STUDIOSLAB_WINDOWROW_H

#include "Graphics/Window/SlabWindow.h"

#include <list>


namespace Slab::Graphics {

    class WindowRow : public SlabWindow {
        struct WinMetaData {
            Pointer<SlabWindow> window;
            DevFloat width;
        };

        std::list<WinMetaData> windowsList;

        RealVector _widthsVector() const;

        bool assertConsistency() const;

    public:
        explicit WindowRow(Str title="", Int flags = 0x0);

        enum RelativePosition {
            Left, Right
        };

        bool addWindow(const Pointer<SlabWindow> &window, RelativePosition= Right, float windowWidth = -1);

        void removeWindow(const Pointer<SlabWindow> &window);

        void arrangeWindows();

        void draw() override;

        void notifyReshape(int w, int h) override;

        bool notifyMouseMotion(int x, int y, int dx, int dy) override;

        bool notifyMouseButton(MouseButton button, KeyState state,
                               ModKeys keys) override;

        bool notifyMouseWheel(double dx, double dy) override;

        bool notifyKeyboard(KeyMap key, KeyState state, ModKeys modKeys) override;

        auto setx(int x) -> void override;

        auto sety(int y) -> void override;

    };


}

#endif //STUDIOSLAB_WINDOWROW_H
