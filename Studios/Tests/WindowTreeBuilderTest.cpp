//
// Created by joao on 30/08/2021.
//

#include "WindowTreeBuilderTest.h"

#include "Graphics/Window/WindowTree/WindowTreeBuilder.h"
#include "GLFreeTypeTests.h"
#include "Graphics/Plot2D/Plot2DWindow.h"
#include "Math/Function/RtoR/Model/FunctionsCollection/Oscillons/AnalyticOscillon.h"
// #include "FourierTestWindow.h"

using namespace Slab;

WindowTreeBuilderTest::WindowTreeBuilderTest()
: Slab::Graphics::SlabWindow({"Tree builder test"})
, osc(0.0, -0.5, 0.75, 0.15, false, false)  {
    Graphics::WindowTreeBuilder treeBuilder(Graphics::WindowStyle::default_window_rect);

    Graphics::SlabWindow *window = nullptr;

    window = new GLFreeTypeTests;
    treeBuilder.addWindow(window, Graphics::WindowTreeBuilder::Above);

    window = new Graphics::SlabWindow;
    treeBuilder.addWindow(window, Graphics::WindowTreeBuilder::Right);

    window = new Graphics::SlabWindow;
    treeBuilder.addWindow(window, Graphics::WindowTreeBuilder::Right);

    // window = new Tests::FourierTestWindow();
    // treeBuilder.addWindow(window, Graphics::WindowTreeBuilder::Above);

    main = treeBuilder.getRoot();
}


void WindowTreeBuilderTest::draw() {
    static Timer timer;

    osc.set_t(timer.getElTime_sec());

    main->draw();
}

void WindowTreeBuilderTest::notifyReshape(int width, int height) {
    Graphics::SlabWindow::notifyReshape(width, height);
    main->notifyReshape(width,  height);

    main->arrange();
}
