//
// Created by joao on 29/08/2021.
//

#include "TestsApp.h"

#include "Core/Backend/Program/DummyProgram.h"
#include "Core/Controller/CLArgsManager.h"

#include "BezierTests.h"
#include "WindowPanelTest.h"
#include "WindowTreeBuilderTest.h"
#include "GLFreeTypeTests.h"
#include "FourierTestWindow.h"
#include "ModernGLTests.h"
#include "Core/Backend/BackendManager.h"
#include "Core/Backend/SFML/SFMLBackend.h"
#include "NuklearTests.h"
#include "Graph3DTests.h"


#define DONT_REGISTER false

TestsApp:: TestsApp(int argc, const char**argv) : AppBase(argc, argv, DONT_REGISTER) {

    AppBase::registerToManager();

    CLArgsManager::Parse();
}


int TestsApp::run() {
    Core::GUIEventListener *test;

    GraphicBackend *backend = nullptr;

    if(true) {
        Core::BackendManager::Startup(Core::GLFW);

        if(true)       test = new Tests::Graph3DTests;
        else if(true)  test = new Tests::ModernGLTests;
        else if(false) test = new Tests::BezierTests;
        else if(true)  test = new Tests::FourierTestWindow;
        else if(true)  test = new WindowTreeBuilderTest;
        else if(true)  test = new WindowPanelTest;
        else test = new GLFreeTypeTests;

        backend = &Core::BackendManager::GetGUIBackend();
        backend->addEventListener(Core::GUIEventListener_ptr(test));

    } else {
        Core::BackendManager::Startup(Core::GLFW);
        auto &guiBackend = Core::BackendManager::GetGUIBackend();
        backend = &guiBackend;
        guiBackend.addEventListener(Core::GUIEventListener_ptr(new Tests::NuklearTests()));
    }

    backend->run(new DummyProgram);

    return 0;
}
