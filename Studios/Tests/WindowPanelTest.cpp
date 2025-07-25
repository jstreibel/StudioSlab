//
// Created by joao on 30/08/2021.
//


#include "WindowPanelTest.h"

#include "Graphics/Window/SlabWindow.h"
#include "GLFreeTypeTests.h"
//#include <Studios/Graphics/Graph.h>

WindowPanelTest::WindowPanelTest()
: FSlabWindow(Slab::Graphics::FSlabWindowConfig{"Panels test"})
, panel(new Slab::Graphics::WindowPanel(Slab::Graphics::FSlabWindowConfig("Big Panellio"))) {
    FSlabWindow *window = nullptr;

    window = new FSlabWindow(Slab::Graphics::FSlabWindowConfig{"Main Window"});
    typedef Slab::TPointer<FSlabWindow> Ptr;
    panel->AddWindow(Ptr(window));

    if(true) {
        using Cfg = Slab::Graphics::FSlabWindowConfig;

        window = new FSlabWindow(Cfg{"Win01"});
        panel->AddWindow(Ptr(window));

        window = new FSlabWindow(Cfg{"Win02"});
        panel->AddWindow(Ptr(window));

        window = new FSlabWindow(Cfg{"Win03"});
        panel->AddWindow(Ptr(window), true);

        window = new FSlabWindow(Cfg{"Win04"});
        window = new GLFreeTypeTests;
        panel->AddWindow(Ptr(window));

        window = new FSlabWindow(Cfg{"Win05"});
        panel->AddWindow(Ptr(window), true, 0.2);

        window = new FSlabWindow(Cfg{"Win06"});
        panel->AddWindow(Ptr(window));

        window = new FSlabWindow(Cfg{"Win07"});
        panel->AddWindow(Ptr(window));

        window = new FSlabWindow(Cfg{"Win08"});
        panel->AddWindow(Ptr(window));

        panel->SetColumnRelativeWidth(0, 0.2);
    }

    panel->ArrangeWindows();
}


void WindowPanelTest::ImmediateDraw(const Slab::Graphics::FPlatformWindow& PlatformWindow) {
    panel->ImmediateDraw(PlatformWindow);
}

void WindowPanelTest::NotifyReshape(int width, int height) {
    panel->NotifyReshape(width, height);

    panel->ArrangeWindows();
}

