//
// Created by joao on 30/08/2021.
//

#ifndef V_SHAPE_WINDOWTREEBUILDERTEST_H
#define V_SHAPE_WINDOWTREEBUILDERTEST_H


#include <Studios/Backend/Graphic/GLUTEventListener.h>
#include <Studios/Graphics/WindowManagement/WindowTree/NodeWindow.h>
//#include <Studios/Graphics/deprecated/Graph_depr.h>

class WindowTreeBuilderTest : public Base::GLUTEventListener {
    //Graph_depr graph;

    NodeWindow *main;
public:
    WindowTreeBuilderTest();
    void draw() override;

    // void notifyReshape(int width, int height) override;
    void notifyReshape(int width, int height) override;

};


#endif //V_SHAPE_WINDOWTREEBUILDERTEST_H
