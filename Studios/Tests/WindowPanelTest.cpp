//
// Created by joao on 30/08/2021.
//


#include "WindowPanelTest.h"

#include "Graphics/Window/SlabWindow.h"
#include "GLFreeTypeTests.h"
//#include <Studios/Graphics/Graph.h>

WindowPanelTest::WindowPanelTest()
: FSlabWindow(Config{"Panels test"}), panel(new Slab::Graphics::WindowPanel) {
    FSlabWindow *window = nullptr;

    window = new FSlabWindow();
    typedef Slab::Pointer<FSlabWindow> Ptr;
    panel->AddWindow(Ptr(window));

    if(true) {
        window = new FSlabWindow();
        panel->AddWindow(Ptr(window));

        window = new FSlabWindow();
        panel->AddWindow(Ptr(window));

        window = new FSlabWindow();
        panel->AddWindow(Ptr(window), true);

        window = new FSlabWindow();
        window = new GLFreeTypeTests;
        panel->AddWindow(Ptr(window));

        window = new FSlabWindow();
        panel->AddWindow(Ptr(window), true, 0.2);

        window = new FSlabWindow();
        panel->AddWindow(Ptr(window));

        window = new FSlabWindow();
        panel->AddWindow(Ptr(window));

        window = new FSlabWindow();
        panel->AddWindow(Ptr(window));

        panel->SetColumnRelativeWidth(0, 0.2);
    }

    panel->arrangeWindows();
}


void WindowPanelTest::ImmediateDraw() {
    panel->ImmediateDraw();
}

void WindowPanelTest::NotifyReshape(int width, int height) {
    panel->NotifyReshape(width, height);

    panel->arrangeWindows();
}

