//
// Created by joao on 30/08/2021.
//


#include "WindowPanelTest.h"

#include "Base/Graphics/Window/Window.h"
//#include <Studios/Graphics/Graph.h>


WindowPanelTest::WindowPanelTest() : panel(new WindowPanel) {
    Window *window = nullptr;

    window = new Window();
    panel->addWindow(window);

    window = new Window();
    panel->addWindow(window);

    window = new Window();
    panel->addWindow(window);

    window = new Window();
    //window->addArtist(new Graph(-1, 1, -1, 1));
    panel->addWindow(window, true);

    window = new Window();
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

    panel->arrangeWindows();
}


void WindowPanelTest::draw() {
    panel->draw();
}

void WindowPanelTest::notifyReshape(int width, int height) {
    panel->setSize(width, height);

    panel->arrangeWindows();
}

