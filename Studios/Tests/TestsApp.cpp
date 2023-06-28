//
// Created by joao on 29/08/2021.
//

#include "TestsApp.h"

#include "WindowPanelTest.h"
//#include "WindowTreeBuilderTest.h"

#include <Base/Backend/GLUT/GLUTBackend.h>
#include <Base/Backend/DummyProgram.h>


TestsApp:: TestsApp(int argc, const char**argv) : AppBase(argc, argv) {}


int TestsApp::run() {
    //auto view = WindowTreeBuilderTest();
    auto view = std::shared_ptr<WindowPanelTest>();

    auto pDummyProgram = new DummyProgram;

    auto backend = GLUTBackend::GetInstance();

    backend->addWindow(view);

    backend->run(pDummyProgram);

    return 0;
}
