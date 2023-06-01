//
// Created by joao on 03/09/2021.
//

#include "WindowTreeBuilder.h"

WindowTreeBuilder::WindowTreeBuilder(int x, int y, int w, int h)
: root(new NodeWindow(x, y, w, h)), shape(x, y, w, h), currentNode(new NodeWindow(x, y, w, h))
{
    root->addSubWindow(currentNode);
}

void WindowTreeBuilder::addWindow(Window *window, WindowTreeBuilder::Location location) {
    auto arr = location==Right?NodeWindow::Horizontal:NodeWindow::Vertical;

    if(currentNode->arrangement != arr){
        auto n = currentNode->children.size();
        if(n <= 1)
            currentNode->arrangement = arr;
        else {
            NodeWindow *newNode = new NodeWindow(nullptr, arr);
            newNode->addSubWindow(currentNode);
            currentNode = newNode;
        }
    }

    currentNode->addSubWindow(window);
}

NodeWindow* WindowTreeBuilder::getRoot(){
    currentNode->x = shape.getx();
    currentNode->y = shape.gety();
    currentNode->w = shape.getw();
    currentNode->h = shape.geth();

    currentNode->arrange();

    return currentNode;
}
