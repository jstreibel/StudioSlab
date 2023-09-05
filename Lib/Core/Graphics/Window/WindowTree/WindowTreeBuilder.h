//
// Created by joao on 03/09/2021.
//

#pragma clang diagnostic push
#pragma ide diagnostic ignored "OCUnusedGlobalDeclarationInspection"
#ifndef V_SHAPE_WINDOWTREEBUILDER_H
#define V_SHAPE_WINDOWTREEBUILDER_H


#include "Core/Graphics/Window/Window.h"
#include "NodeWindow.h"

class WindowTreeBuilder {
    Window shape;
    NodeWindow *root;
    NodeWindow *currentNode;

    WindowTreeBuilder() = delete;
public:
    WindowTreeBuilder(int x, int y, int w, int h);

    enum Location {Above=Vertical, Right=Horizontal};

    void addWindow(Window *window, Location location);
    NodeWindow* getRoot();
};


#endif //V_SHAPE_WINDOWTREEBUILDER_H

#pragma clang diagnostic pop