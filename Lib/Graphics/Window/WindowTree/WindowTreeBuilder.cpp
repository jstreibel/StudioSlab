//
// Created by joao on 03/09/2021.
//

#include "WindowTreeBuilder.h"


namespace Slab::Graphics {

    WindowTreeBuilder::WindowTreeBuilder(int x, int y, int w, int h)
            : root(new NodeWindow(x, y, w, h)), shape(x, y, w, h),
              currentNode(new NodeWindow(x, y, w, h)) {
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