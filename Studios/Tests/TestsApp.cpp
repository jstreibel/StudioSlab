//
// Created by joao on 29/08/2021.
//

#include "TestsApp.h"

#include "BezierTests.h"
#include "WindowPanelTest.h"
#include "WindowTreeBuilderTest.h"
#include "GLFreeTypeTests.h"
#include "FourierTestWindow.h"
#include "Core/Controller/CLArgsManager.h"

#include <Core/Backend/GLUT/GLUTBackend.h>
#include <Core/Backend/DummyProgram.h>

#define DONT_REGISTER false

TestsApp:: TestsApp(int argc, const char**argv) : AppBase(argc, argv, DONT_REGISTER) {
    Backend::Initialize<GLUTBackend>();

    AppBase::registerToManager();

    CLArgsManager::GetInstance()->Parse();
}


int TestsApp::run() {
    Window *test;
    if(false)      test = new Tests::BezierTests;
    else if(true) test = new Tests::FourierTestWindow;
    else if(true) test = new WindowTreeBuilderTest;
    else if(true) test = new WindowPanelTest;
    else test = new GLFreeTypeTests;

    auto &backend = GLUTBackend::GetInstance();

    backend.addWindow(Window::Ptr(test));

    backend.run(new DummyProgram);

    return 0;
}
