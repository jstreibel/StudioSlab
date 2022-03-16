//
// Created by joao on 29/08/2021.
//

#include "TestsApp.h"

#include "WindowPanelTest.h"
//#include "WindowTreeBuilderTest.h"

#include <Studios/Backend/GLUT/GLUTBackend.h>
#include <Studios/Backend/DummyProgram.h>


TestsApp:: TestsApp(int argc, const char**argv) : AppBase(argc, argv) {}


int TestsApp::run() {
    //auto view = WindowTreeBuilderTest();
    auto view = WindowPanelTest();
    auto pDummyProgram = new DummyProgram;

    auto backend = GLUTBackend::GetInstance();

    backend->setOpenGLOutput(&view);

    backend->run(pDummyProgram);

    return 0;
}
