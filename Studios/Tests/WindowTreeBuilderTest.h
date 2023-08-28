//
// Created by joao on 30/08/2021.
//

#ifndef V_SHAPE_WINDOWTREEBUILDERTEST_H
#define V_SHAPE_WINDOWTREEBUILDERTEST_H


#include "Base/Backend/Events/EventListener.h"
#include "Base/Graphics/Window/WindowTree/NodeWindow.h"
//#include <Studios/Graphics/deprecated/Graph_depr.h>

class WindowTreeBuilderTest : public Window {
    //Graph_depr graph;

    NodeWindow *main;
public:
    WindowTreeBuilderTest();
    void draw() override;

    void notifyReshape(int width, int height) override;

    bool notifyScreenReshape(int newScreenWidth, int newScreenHeight) override;

};


#endif //V_SHAPE_WINDOWTREEBUILDERTEST_H
