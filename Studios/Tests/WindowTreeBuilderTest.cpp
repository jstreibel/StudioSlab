//
// Created by joao on 30/08/2021.
//

#include "WindowTreeBuilderTest.h"

#include "Core/Graphics/Window/WindowTree/WindowTreeBuilder.h"
#include "GLFreeTypeTests.h"
#include "Math/Graph/Graph.h"
#include "Mappings/RtoR/View/Graphics/GraphRtoR.h"
#include "Mappings/RtoR/Model/FunctionsCollection/Oscillons/AnalyticOscillon.h"


WindowTreeBuilderTest::WindowTreeBuilderTest()
: osc(0.0, -0.5, 0.75, 0.15, false, false)  {
    WindowTreeBuilder treeBuilder(0, 0, 1500, 1500);

    Window *window = nullptr;

    if(true) {
        auto graph = new GraphRtoR(-1, 1, -1, 1, "Graph");
        graph->addFunction(&osc, "oscillon 1");
        graph->setScale(10);
        window = graph;
    } else {
        window = new Window;
    }
    treeBuilder.addWindow(window, WindowTreeBuilder::Right);

    window = new Window();
    treeBuilder.addWindow(window, WindowTreeBuilder::Right);

    window = new GLFreeTypeTests;
    treeBuilder.addWindow(window, WindowTreeBuilder::Above);

    window = new GLFreeTypeTests;
    treeBuilder.addWindow(window, WindowTreeBuilder::Above);

    if(true) {
        auto graph = new GraphRtoR(-1, 1, -1, 1, "Graph");
        graph->addFunction(&osc, "oscillon 2");
        graph->setScale(10);
        window = graph;
    } else {
        window = new Window;
    }
    treeBuilder.addWindow(window, WindowTreeBuilder::Right);

    window = new Window();
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

bool WindowTreeBuilderTest::notifyScreenReshape(int newScreenWidth, int newScreenHeight) {
    notifyReshape(newScreenWidth, newScreenHeight);

    return true;
}
