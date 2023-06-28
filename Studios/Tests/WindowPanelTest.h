//
// Created by joao on 30/08/2021.
//

#ifndef V_SHAPE_WINDOWTREEBUILDERTEST_H
#define V_SHAPE_WINDOWTREEBUILDERTEST_H


#include <Base/Backend/Events/EventListener.h>
#include <Base/Graphics/Window/WindowContainer/WindowPanel.h>


class WindowPanelTest : public Window {
    WindowPanel *panel;

public:
    WindowPanelTest();
    void draw() override;

    void notifyReshape(int width, int height) override;

};


#endif //V_SHAPE_WINDOWTREEBUILDERTEST_H
