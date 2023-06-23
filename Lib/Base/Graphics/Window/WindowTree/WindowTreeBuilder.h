//
// Created by joao on 03/09/2021.
//

#pragma clang diagnostic push
#pragma ide diagnostic ignored "OCUnusedGlobalDeclarationInspection"
#ifndef V_SHAPE_WINDOWTREEBUILDER_H
#define V_SHAPE_WINDOWTREEBUILDER_H


#include "Base/Graphics/Window/Window.h"
#include "NodeWindow.h"

class WindowTreeBuilder {
    Window shape;
    NodeWindow *root = nullptr;
    NodeWindow *currentNode = nullptr;

public:
    WindowTreeBuilder(int x, int y, int w, int h);

    enum Location {Above=NodeWindow::Vertical, Right=NodeWindow::Horizontal};

    void addWindow(Window *window, Location location);
    NodeWindow* getRoot();
};


#endif //V_SHAPE_WINDOWTREEBUILDER_H

#pragma clang diagnostic pop