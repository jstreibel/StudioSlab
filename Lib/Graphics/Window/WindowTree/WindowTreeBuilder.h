//
// Created by joao on 03/09/2021.
//

#pragma clang diagnostic push
#pragma ide diagnostic ignored "OCUnusedGlobalDeclarationInspection"
#ifndef V_SHAPE_WINDOWTREEBUILDER_H
#define V_SHAPE_WINDOWTREEBUILDER_H


#include "Graphics/Window/SlabWindow.h"
#include "NodeWindow.h"


namespace Slab::Graphics {

    class WindowTreeBuilder {
        RectI shape;
        NodeWindow *root;
        NodeWindow *currentNode;

        WindowTreeBuilder() = delete;

    public:
        WindowTreeBuilder(RectI window_rect = Graphics::default_window_rect);

        enum Location {
            Above = Vertical, Right = Horizontal
        };

        void addWindow(SlabWindow *window, Location location);

        NodeWindow *getRoot();
    };

}

#endif //V_SHAPE_WINDOWTREEBUILDER_H

#pragma clang diagnostic pop