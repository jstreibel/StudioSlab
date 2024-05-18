//
// Created by joao on 30/08/2021.
//

#ifndef V_SHAPE_WINDOWPANELTEST_H
#define V_SHAPE_WINDOWPANELTEST_H


#include <Core/Backend/Events/GUIEventListener.h>
#include "Graphics/Window/Window.h"
#include "Graphics/Window/WindowContainer/WindowPanel.h"


class WindowPanelTest : public Graphics::Window {
    Graphics::WindowPanel *panel;

public:
    WindowPanelTest();
    void draw() override;

    void notifyReshape(int width, int height) override;

};


#endif //V_SHAPE_WINDOWPANELTEST_H
