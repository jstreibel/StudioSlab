//
// Created by joao on 02/09/2021.
//

#ifndef V_SHAPE_NODEWINDOW_H
#define V_SHAPE_NODEWINDOW_H

#include "Graphics/Window/SlabWindow.h"

#include <vector>


namespace Slab::Graphics {

    typedef Vector<SlabWindow *> WindowContainer;

    class WindowTreeBuilder;

    enum NodeArrangement {
        Horizontal, Vertical
    };

    class NodeWindow : public SlabWindow {
        SlabWindow *parent = nullptr;
        NodeArrangement arrangement = Horizontal;
        WindowContainer children;

    public:
        friend WindowTreeBuilder;

        NodeWindow(int x, int y, int w, int h);

        explicit NodeWindow(NodeWindow *parent, NodeArrangement arrangement = Horizontal);

        explicit NodeWindow(const SlabWindow &window);

        void addSubWindow(SlabWindow *subWindow);

        void arrange();

        void draw() override;

        void setupParentSystemWindowHeight(Int anInt) override;

    };

}

#endif //V_SHAPE_NODEWINDOW_H
