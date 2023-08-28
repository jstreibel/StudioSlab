//
// Created by joao on 02/09/2021.
//

#include "NodeWindow.h"


NodeWindow::NodeWindow(const Window &window) : Window(window) {
}

NodeWindow::NodeWindow(int x, int y, int w, int h) : Window(x, y, w, h) {

}

NodeWindow::NodeWindow(NodeWindow *parent, NodeArrangement nodeArrangement) : parent(parent), arrangement(nodeArrangement) {

}

void NodeWindow::addSubWindow(Window *subWindow) {
    NodeWindow *nodeSub = dynamic_cast<NodeWindow*>(subWindow);

    if(nodeSub != nullptr) nodeSub->parent = this;

    subWindow->setClear(false);
    children.emplace_back(subWindow);
}

void NodeWindow::arrange() {
    auto n = children.size();

    auto childWidth  = w;
    auto childHeight = h;

    auto dx = childWidth/n;
    auto dy = childHeight/n;

    if(arrangement == Horizontal){
        childWidth /= n;
        dy = 0;
    } else {
        childHeight /= n;
        dx = 0;
    }

    for(auto i=0; i<n; ++i){
        auto child = children[i];

        child->setx(x + i*dx);
        child->sety(y + i*dy);
        child->setSize(childWidth, childHeight);

        auto cast = dynamic_cast<NodeWindow*>(child);
        if(cast != nullptr) cast->arrange();
    }
}

void NodeWindow::draw() {
    if(children.empty()) Window::draw( );

    for(auto win : children)
        win->draw();
}






