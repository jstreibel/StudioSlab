//
// Created by joao on 30/08/2021.
//


#include "WindowPanelTest.h"

#include "Graphics/Window/Window.h"
#include "GLFreeTypeTests.h"
//#include <Studios/Graphics/Graph.h>

WindowPanelTest::WindowPanelTest() : panel(new Graphics::WindowPanel) {
    Window *window = nullptr;

    window = new Window();
    panel->addWindow(window);

    if(true) {
        window = new Window();
        panel->addWindow(window);

        window = new Window();
        panel->addWindow(window);

        window = new Window();
        panel->addWindow(window, true);

        window = new Window();
        window = new GLFreeTypeTests;
        panel->addWindow(window);

        window = new Window();
        panel->addWindow(window, true, 0.2);

        window = new Window();
        panel->addWindow(window);

        window = new Window();
        panel->addWindow(window);

        window = new Window();
        panel->addWindow(window);

        panel->setColumnRelativeWidth(0, 0.2);
    }

    panel->arrangeWindows();
}


void WindowPanelTest::draw() {
    panel->draw();
}

void WindowPanelTest::notifyReshape(int width, int height) {
    panel->setSize(width, height);

    panel->arrangeWindows();
}

