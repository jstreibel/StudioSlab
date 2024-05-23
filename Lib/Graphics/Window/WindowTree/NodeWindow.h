//
// Created by joao on 02/09/2021.
//

#ifndef V_SHAPE_NODEWINDOW_H
#define V_SHAPE_NODEWINDOW_H

#include "Graphics/Window/Window.h"

#include <vector>


namespace Slab::Graphics {

    typedef std::vector<Window *> WindowContainer;

    class WindowTreeBuilder;

    enum NodeArrangement {
        Horizontal, Vertical
    };

    class NodeWindow : public Window {
        Window *parent = nullptr;
        NodeArrangement arrangement = Horizontal;
        WindowContainer children;

    public:
        friend WindowTreeBuilder;

        NodeWindow(int x, int y, int w, int h);

        explicit NodeWindow(NodeWindow *parent, NodeArrangement arrangement = Horizontal);

        explicit NodeWindow(const Window &window);

        void addSubWindow(Window *subWindow);

        void arrange();

        void draw() override;

    };

}

#endif //V_SHAPE_NODEWINDOW_H
