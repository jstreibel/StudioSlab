//
// Created by joao on 30/08/2021.
//


#include "WindowPanelTest.h"

#include "Graphics/Window/SlabWindow.h"
#include "GLFreeTypeTests.h"
//#include <Studios/Graphics/Graph.h>

WindowPanelTest::WindowPanelTest() : panel(new Slab::Graphics::WindowPanel) {
    SlabWindow *window = nullptr;

    window = new SlabWindow();
    typedef Slab::Pointer<SlabWindow> Ptr;
    panel->addWindow(Ptr(window));

    if(true) {
        window = new SlabWindow();
        panel->addWindow(Ptr(window));

        window = new SlabWindow();
        panel->addWindow(Ptr(window));

        window = new SlabWindow();
        panel->addWindow(Ptr(window), true);

        window = new SlabWindow();
        window = new GLFreeTypeTests;
        panel->addWindow(Ptr(window));

        window = new SlabWindow();
        panel->addWindow(Ptr(window), true, 0.2);

        window = new SlabWindow();
        panel->addWindow(Ptr(window));

        window = new SlabWindow();
        panel->addWindow(Ptr(window));

        window = new SlabWindow();
        panel->addWindow(Ptr(window));

        panel->setColumnRelativeWidth(0, 0.2);
    }

    panel->arrangeWindows();
}


void WindowPanelTest::draw() {
    panel->draw();
}

void WindowPanelTest::notifyReshape(int width, int height) {
    panel->notifyReshape(width, height);

    panel->arrangeWindows();
}

