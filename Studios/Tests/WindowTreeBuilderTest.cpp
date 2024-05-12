//
// Created by joao on 30/08/2021.
//

#include "WindowTreeBuilderTest.h"

#include "Graphics/Window/WindowTree/WindowTreeBuilder.h"
#include "GLFreeTypeTests.h"
#include "Graphics/Graph/Graph.h"
#include "Graphics/Graph/ℝ↦ℝ/GraphRtoR.h"
#include "Math/Function/Maps/RtoR/Model/FunctionsCollection/Oscillons/AnalyticOscillon.h"
#include "FourierTestWindow.h"


WindowTreeBuilderTest::WindowTreeBuilderTest()
: osc(0.0, -0.5, 0.75, 0.15, false, false)  {
    Graphics::WindowTreeBuilder treeBuilder(0, 0, 1500, 1500);

    Graphics::Window *window = nullptr;

    window = new GLFreeTypeTests;
    treeBuilder.addWindow(window, Graphics::WindowTreeBuilder::Above);

    window = new Graphics::Window;
    treeBuilder.addWindow(window, Graphics::WindowTreeBuilder::Right);

    window = new Graphics::Window;
    treeBuilder.addWindow(window, Graphics::WindowTreeBuilder::Right);

    window = new Tests::FourierTestWindow();
    treeBuilder.addWindow(window, Graphics::WindowTreeBuilder::Above);

    main = treeBuilder.getRoot();
}


void WindowTreeBuilderTest::draw() {
    static Timer timer;

    osc.set_t(timer.getElTime_sec());

    main->draw();
}

void WindowTreeBuilderTest::notifyReshape(int width, int height) {
    Graphics::Window::notifyReshape(width, height);
    main->setSize(width,  height);

    main->arrange();
}
