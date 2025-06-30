//
// Created by joao on 20/08/23.
//

#ifndef STUDIOSLAB_WINDOWROW_H
#define STUDIOSLAB_WINDOWROW_H

#include "Graphics/Window/SlabWindow.h"

#include <list>


namespace Slab::Graphics {

    class WindowRow : public FSlabWindow {
        struct WinMetaData {
            Pointer<FSlabWindow> window;
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

        bool AddWindow(const Pointer<FSlabWindow> &window, RelativePosition= Right, float windowWidth = -1);

        void removeWindow(const Pointer<FSlabWindow> &window);

        void arrangeWindows();

        void ImmediateDraw() override;

        void NotifyReshape(int w, int h) override;

        bool NotifyMouseMotion(int x, int y, int dx, int dy) override;

        bool NotifyMouseButton(EMouseButton button, EKeyState state,
                               EModKeys keys) override;

        bool NotifyMouseWheel(double dx, double dy) override;

        bool NotifyKeyboard(EKeyMap key, EKeyState state, EModKeys modKeys) override;

        auto Set_x(int x) -> void override;

        auto Set_y(int y) -> void override;

    };


}

#endif //STUDIOSLAB_WINDOWROW_H
