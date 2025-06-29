
//
// Created by joao on 02/09/2021.
//

#include "NodeWindow.h"


namespace Slab::Graphics {

    NodeWindow::NodeWindow(const FSlabWindow &window) : FSlabWindow(window) {
    }

    NodeWindow::NodeWindow(RectI window_rect) : FSlabWindow({"", window_rect}) {

    }

    NodeWindow::NodeWindow(NodeWindow *parent, NodeArrangement nodeArrangement) : parent(parent),
                                                                                  arrangement(
                                                                                          nodeArrangement) {

    }

    void NodeWindow::addSubWindow(FSlabWindow *subWindow) {
        NodeWindow *nodeSub = dynamic_cast<NodeWindow *>(subWindow);

        if (nodeSub != nullptr) {
            nodeSub->parent = this;
            nodeSub->setClear(false);
        }

        children.emplace_back(subWindow);
    }

    void NodeWindow::arrange() {
        auto n = children.size();

        auto childWidth = GetWidth();
        auto childHeight = GetHeight();

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

            child->Set_x(getx() + i * dx);
            child->Set_y(gety() + i * dy);
            child->NotifyReshape(childWidth, childHeight);

            auto cast = dynamic_cast<NodeWindow *>(child);
            if (cast != nullptr) cast->arrange();
        }
    }

    void NodeWindow::Draw() {
        if (children.empty()) FSlabWindow::Draw();

        for (auto win: children)
            win->Draw();
    }

}