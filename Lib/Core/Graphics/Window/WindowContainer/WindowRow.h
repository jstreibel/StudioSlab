//
// Created by joao on 20/08/23.
//

#ifndef STUDIOSLAB_WINDOWROW_H
#define STUDIOSLAB_WINDOWROW_H

#include "Core/Graphics/Window/Window.h"

#include <list>

class WindowRow : public Window {
    struct WinMetaData {
        Window::Ptr window;
        Real width;
    };

    std::list<WinMetaData> windowsList;
    RealVector _widthsVector() const;

    bool assertConsistency() const;
public:
    WindowRow() = default;

    enum RelativePosition { Left, Right };
    bool addWindow(const Window::Ptr& window, RelativePosition=Right, float windowWidth=-1);
    void removeWindow(const Window::Ptr& window);

    void arrangeWindows();

    void draw() override;

    void notifyReshape(int w, int h) override;

    bool notifyMouseMotion(int x, int y)        override;
    bool notifyMouseButton(Core::MouseButton button, Core::KeyState state, Core::ModKeys keys) override;
    bool notifyMouseWheel(double dx, double dy) override;

    bool notifyKeyboard(Core::KeyMap key, Core::KeyState state, Core::ModKeys modKeys) override;

    bool notifyFilesDropped(StrVector paths) override;

};


#endif //STUDIOSLAB_WINDOWROW_H
