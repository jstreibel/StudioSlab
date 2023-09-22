//
// Created by joao on 20/08/23.
//

#ifndef STUDIOSLAB_WINDOWROW_H
#define STUDIOSLAB_WINDOWROW_H

#include "Core/Graphics/Window/Window.h"


class WindowRow : public Window {
    std::vector<Window::Ptr> windows;
    RealVector widths;

    bool assertConsistency() const;
public:
    WindowRow() = default;

    void addWindow(Window::Ptr window, float windowWidth=-1);

    void arrangeWindows();

    void draw() override;

    void notifyReshape(int newWinW, int newWinH) override;

    bool notifyMouseMotion(int x, int y)        override;
    bool notifyMouseButton(Core::MouseButton button, Core::KeyState state, Core::ModKeys keys) override;
    bool notifyMouseWheel(double dx, double dy) override;

    bool notifyKeyboard(Core::KeyMap key, Core::KeyState state, Core::ModKeys modKeys) override;

    bool notifyFilesDropped(StrVector paths) override;

};


#endif //STUDIOSLAB_WINDOWROW_H
