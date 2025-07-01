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
: FSlabWindow(FConfig{"Tree builder test"})
, osc(0.0, -0.5, 0.75, 0.15, false, false)  {
    Graphics::WindowTreeBuilder treeBuilder(Graphics::WindowStyle::DefaultWindowRect);

    Graphics::FSlabWindow *window = nullptr;

    window = new GLFreeTypeTests;
    treeBuilder.addWindow(window, Graphics::WindowTreeBuilder::Above);

    window = new Graphics::FSlabWindow;
    treeBuilder.addWindow(window, Graphics::WindowTreeBuilder::Right);

    window = new Graphics::FSlabWindow;
    treeBuilder.addWindow(window, Graphics::WindowTreeBuilder::Right);

    // window = new Tests::FourierTestWindow();
    // treeBuilder.addWindow(window, Graphics::WindowTreeBuilder::Above);

    main = treeBuilder.getRoot();
}


void WindowTreeBuilderTest::ImmediateDraw() {
    static Timer timer;

    osc.set_t(timer.GetElapsedTime_Seconds());

    main->ImmediateDraw();
}

void WindowTreeBuilderTest::NotifyReshape(int width, int height) {
    Graphics::FSlabWindow::NotifyReshape(width, height);
    main->NotifyReshape(width,  height);

    main->arrange();
}
