//
// Created by joao on 30/08/2021.
//

#ifndef V_SHAPE_WINDOWTREEBUILDERTEST_H
#define V_SHAPE_WINDOWTREEBUILDERTEST_H


#include <Studios/Backend/GLUT/GLUTEventListener.h>
#include <Studios/Graphics/WindowManagement/WindowContainer/WindowPanel.h>


class WindowPanelTest : public Base::GLUTEventListener {
    WindowPanel *panel;

public:
    WindowPanelTest();
    void draw() override;

    void notifyReshape(int width, int height) override;

};


#endif //V_SHAPE_WINDOWTREEBUILDERTEST_H
