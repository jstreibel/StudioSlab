//
// Created by joao on 29/08/2021.
//

#include "TestsApp.h"

#include "Core/App/AppBase.h"
#include "Core/Controller/CommandLine/CLArgsManager.h"

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
#include "VShapeExpansionTest.h"


#define DONT_REGISTER false

TestsApp:: TestsApp(int argc, const char**argv) : Slab::Core::AppBase(argc, argv, DONT_REGISTER) {

    AppBase::registerToManager();

    Slab::Core::CLArgsManager::Parse();
}


int TestsApp::run() {
    using namespace Slab;

    Pointer<Core::GUIEventListener> test;

    Core::GraphicBackend *backend = nullptr;

    if(true) {
        Core::BackendManager::Startup(Core::GLFW);

        if(true)      test = New<Tests::VShapeExpansionTest>();
        else if(true)  test = New<Tests::Graph3DTests>();
        else if(true)  test = New<Tests::ModernGLTests>();
        // else if(false) test = new Tests::BezierTests;
        // else if(true)  test = new Tests::FourierTestWindow;
        // else if(true)  test = new WindowTreeBuilderTest;
        // else if(true)  test = new WindowPanelTest;
        // else test = new GLFreeTypeTests;

        backend = &Core::BackendManager::GetGUIBackend();
        backend->addEventListener(test);

    } else {
        Core::BackendManager::Startup(Core::GLFW);
        auto &guiBackend = Core::BackendManager::GetGUIBackend();
        backend = &guiBackend;
        guiBackend.addEventListener(Core::GUIEventListener_ptr(new Tests::NuklearTests()));
    }

    backend->run();

    return 0;
}
