//
// Created by joao on 29/08/2021.
//

#include "TestsApp.h"

#include "WindowPanelTest.h"
#include "WindowTreeBuilderTest.h"
#include "GLFreeTypeTests.h"

#include <Core/Backend/GLUT/GLUTBackend.h>
#include <Core/Backend/DummyProgram.h>

#define DONT_REGISTER false

TestsApp:: TestsApp(int argc, const char**argv) : AppBase(argc, argv, DONT_REGISTER) {
    Backend::Initialize<GLUTBackend>();

    AppBase::registerToManager();
}


int TestsApp::run() {
    Window *test;
    if(false)      test = new WindowTreeBuilderTest;
    else if(false) test = new WindowPanelTest;
    else test = new GLFreeTypeTests;

    auto &backend = GLUTBackend::GetInstance();

    backend.addWindow(Window::Ptr(test));

    backend.run(new DummyProgram);

    return 0;
}
