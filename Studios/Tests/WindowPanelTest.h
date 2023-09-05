//
// Created by joao on 30/08/2021.
//

#ifndef V_SHAPE_WINDOWPANELTEST_H
#define V_SHAPE_WINDOWPANELTEST_H


#include <Core/Backend/Events/EventListener.h>
#include <Core/Graphics/Window/WindowContainer/WindowPanel.h>


class WindowPanelTest : public Window {
    WindowPanel *panel;

public:
    WindowPanelTest();
    void draw() override;

    void notifyReshape(int width, int height) override;

};


#endif //V_SHAPE_WINDOWPANELTEST_H
