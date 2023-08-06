//
// Created by joao on 30/08/2021.
//

#include "WindowTreeBuilderTest.h"

#include "Base/Graphics/Window/WindowTree/WindowTreeBuilder.h"
//#include <Studios/Graphics/Graph.h>


WindowTreeBuilderTest::WindowTreeBuilderTest()  {
    WindowTreeBuilder treeBuilder(0, 0, 1500, 1500);

    Window *window = nullptr;

    window = new Window();
    treeBuilder.addWindow(window, WindowTreeBuilder::Right);

    window = new Window();
    treeBuilder.addWindow(window, WindowTreeBuilder::Right);

    window = new Window();
    treeBuilder.addWindow(window, WindowTreeBuilder::Above);

    window = new Window();
    treeBuilder.addWindow(window, WindowTreeBuilder::Above);

    window = new Window();
    //auto graph = new Graph();
    //window->addArtist(graph);
    treeBuilder.addWindow(window, WindowTreeBuilder::Right);

    window = new Window();
    treeBuilder.addWindow(window, WindowTreeBuilder::Above);

    main = treeBuilder.getRoot();
}


void WindowTreeBuilderTest::draw() {
    main->draw();
}

void WindowTreeBuilderTest::notifyReshape(int width, int height) {
    Window::notifyReshape(width, height);
    main->setSize(width,  height);

    main->arrange();
}
