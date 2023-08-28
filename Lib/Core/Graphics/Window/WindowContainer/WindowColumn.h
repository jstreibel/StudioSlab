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
    bool notifyMouseMotion(int x, int y)        override;
    bool notifyMousePassiveMotion(int x, int y) override;
    bool notifyMouseWheel(int wheel, int direction, int x, int y) override;
    bool notifyMouseButton(int button, int dir, int x, int y) override;
};


#endif //STUDIOSLAB_WINDOWCOLUMN_H
