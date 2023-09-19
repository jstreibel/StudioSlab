//
// Created by joao on 30/08/2021.
//

#include "WindowTreeBuilderTest.h"

#include "Core/Graphics/Window/WindowTree/WindowTreeBuilder.h"
#include "GLFreeTypeTests.h"
#include "Math/Graph/Graph.h"
#include "Maps/RtoR/View/GraphRtoR.h"
#include "Maps/RtoR/Model/FunctionsCollection/Oscillons/AnalyticOscillon.h"
#include "FourierTestWindow.h"


WindowTreeBuilderTest::WindowTreeBuilderTest()
: osc(0.0, -0.5, 0.75, 0.15, false, false)  {
    WindowTreeBuilder treeBuilder(0, 0, 1500, 1500);

    Window *window = nullptr;

    window = new GLFreeTypeTests;
    treeBuilder.addWindow(window, WindowTreeBuilder::Above);

    window = new Window;
    treeBuilder.addWindow(window, WindowTreeBuilder::Right);

    window = new Window;
    treeBuilder.addWindow(window, WindowTreeBuilder::Right);

    window = new Tests::FourierTestWindow();
    treeBuilder.addWindow(window, WindowTreeBuilder::Above);

    main = treeBuilder.getRoot();
}


void WindowTreeBuilderTest::draw() {
    static Timer timer;

    osc.set_t(timer.getElTime_sec());

    main->draw();
}

void WindowTreeBuilderTest::notifyReshape(int width, int height) {
    Window::notifyReshape(width, height);
    main->setSize(width,  height);

    main->arrange();
}
