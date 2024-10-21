//
// Created by joao on 02/09/2021.
//

#include "NodeWindow.h"


namespace Slab::Graphics {

    NodeWindow::NodeWindow(const SlabWindow &window) : SlabWindow(window) {
    }

    NodeWindow::NodeWindow(RectI window_rect) : SlabWindow(window_rect) {

    }

    NodeWindow::NodeWindow(NodeWindow *parent, NodeArrangement nodeArrangement) : parent(parent),
                                                                                  arrangement(
                                                                                          nodeArrangement) {

    }

    void NodeWindow::addSubWindow(SlabWindow *subWindow) {
        NodeWindow *nodeSub = dynamic_cast<NodeWindow *>(subWindow);

        if (nodeSub != nullptr) {
            nodeSub->parent = this;
            nodeSub->setClear(false);
        }

        children.emplace_back(subWindow);
    }

    void NodeWindow::arrange() {
        auto n = children.size();

        auto childWidth = getw();
        auto childHeight = geth();

        auto dx = childWidth / n;
        auto dy = childHeight / n;

        if (arrangement == Horizontal) {
            childWidth /= n;
            dy = 0;
        } else {
            childHeight /= n;
            dx = 0;
        }

        for (auto i = 0; i < n; ++i) {
            auto child = children[i];

            child->setx(getx() + i * dx);
            child->sety(gety() + i * dy);
            child->notifyReshape(childWidth, childHeight);

            auto cast = dynamic_cast<NodeWindow *>(child);
            if (cast != nullptr) cast->arrange();
        }
    }

    void NodeWindow::draw() {
        if (children.empty()) SlabWindow::draw();

        for (auto win: children)
            win->draw();
    }

    void NodeWindow::setupParentSystemWindowHeight(Int h) {
        SlabWindow::setupParentSystemWindowHeight(h);

        for (auto win: children) win->setupParentSystemWindowHeight(h);

    }

}