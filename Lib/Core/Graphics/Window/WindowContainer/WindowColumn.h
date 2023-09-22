//
// Created by joao on 20/08/23.
//

#ifndef STUDIOSLAB_WINDOWCOLUMN_H
#define STUDIOSLAB_WINDOWCOLUMN_H

#include "Core/Graphics/Window/Window.h"


class WindowColumn : public Window {
    std::vector<Window::Ptr> windows;
    RealVector heights;

    bool assertConsistency() const;
public:
    WindowColumn() = default;

    void addWindow(Window::Ptr window, float windowHeight=-1);

    void arrangeWindows();

    void draw() override;

    void notifyReshape(int newWinW, int newWinH) override;

    bool notifyKeyboard(Core::KeyMap key, Core::KeyState state, Core::ModKeys modKeys) override;

    bool notifyFilesDropped(StrVector paths) override;

    bool notifyMouseMotion(int x, int y)        override;
    bool notifyMouseButton(Core::MouseButton button, Core::KeyState state, Core::ModKeys keys) override;
    bool notifyMouseWheel(double dx, double dy) override;
};


#endif //STUDIOSLAB_WINDOWCOLUMN_H
