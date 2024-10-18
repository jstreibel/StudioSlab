//
// Created by joao on 20/08/23.
//

#ifndef STUDIOSLAB_WINDOWROW_H
#define STUDIOSLAB_WINDOWROW_H

#include "Graphics/Window/Window.h"

#include <list>


namespace Slab::Graphics {

    class WindowRow : public Window {
        struct WinMetaData {
            Window::Ptr window;
            Real width;
        };

        std::list<WinMetaData> windowsList;

        RealVector _widthsVector() const;

        bool assertConsistency() const;

    public:
        WindowRow(Window::Flags flags = None);

        enum RelativePosition {
            Left, Right
        };

        bool addWindow(const Window::Ptr &window, RelativePosition= Right, float windowWidth = -1);

        void removeWindow(const Window::Ptr &window);

        void arrangeWindows();

        void draw() override;

        void notifyReshape(int w, int h) override;

        bool notifyMouseMotion(int x, int y) override;

        bool notifyMouseButton(MouseButton button, KeyState state,
                               ModKeys keys) override;

        bool notifyMouseWheel(double dx, double dy) override;

        bool notifyKeyboard(KeyMap key, KeyState state, ModKeys modKeys) override;

        bool notifyFilesDropped(StrVector paths) override;

    };


}

#endif //STUDIOSLAB_WINDOWROW_H
