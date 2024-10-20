//
// Created by joao on 03/09/2021.
//

#include "WindowTreeBuilder.h"


namespace Slab::Graphics {

    WindowTreeBuilder::WindowTreeBuilder(RectI window_rect)
            : root(new NodeWindow(window_rect)), shape(window_rect),
              currentNode(new NodeWindow(window_rect)) {
        root->addSubWindow(currentNode);
    }

    void WindowTreeBuilder::addWindow(SlabWindow *window, WindowTreeBuilder::Location location) {
        auto arr = location == Right ? Horizontal : Vertical;

        if (currentNode->arrangement != arr) {
            auto n = currentNode->children.size();
            if (n <= 1)
                currentNode->arrangement = arr;
            else {
                auto *newNode = new NodeWindow(nullptr, arr);
                newNode->addSubWindow(currentNode);
                currentNode = newNode;
            }
        }

        currentNode->addSubWindow(window);
    }

    NodeWindow *WindowTreeBuilder::getRoot() {
        auto myRect = shape.getViewport();
        currentNode->setx(myRect.xMin);
        currentNode->sety(myRect.yMin);
        currentNode->setSize(myRect.width(), myRect.height());

        currentNode->arrange();

        return currentNode;
    }


}