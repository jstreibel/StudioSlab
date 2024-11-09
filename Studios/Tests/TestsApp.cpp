//
// Created by joao on 29/08/2021.
//

#include "TestsApp.h"

#include "AppBase.h"
#include "Core/Controller/CommandLine/CLArgsManager.h"
#include "Graphics/Window/SlabWindowManager.h"

#include "BezierTests.h"
#include "WindowPanelTest.h"
#include "WindowTreeBuilderTest.h"
#include "GLFreeTypeTests.h"
#include "FourierTestWindow.h"
#include "ModernGLTests.h"
#include "Core/Backend/BackendManager.h"
#include "Graphics/Backend/SFML/SFMLBackend.h"
#include "NuklearTests.h"
#include "Graph3DTests.h"
#include "VShapeExpansionTest.h"
#include "LaTeXTests.h"
#include "Nodes/ImGuiNodesTest2.h"
#include "Nodes/ImGuiNodesTest3.h"

#define DONT_REGISTER false

TestsApp:: TestsApp(int argc, const char**argv) : Slab::Core::AppBase(argc, argv, DONT_REGISTER) {

    AppBase::registerToManager();

    Slab::Core::CLArgsManager::Parse(argc, argv);
}


int TestsApp::run() {
    using namespace Slab;

    Pointer<Graphics::GraphicBackend> backend = nullptr;

    if(true) {
        Core::BackendManager::Startup("GLFW");

        auto wm = New<Graphics::SlabWindowManager>();

        Pointer<Graphics::SlabWindow> temp;

        temp = New<Tests::LaTeXTests>();
        wm->addSlabWindow(temp);

        temp = New<Tests::VShapeExpansionTest>();
        wm->addSlabWindow(temp);

        temp = New<Tests::Graph3DTests>();
        wm->addSlabWindow(temp);

        temp = New<Tests::ModernGLTests>();
        wm->addSlabWindow(temp);

        temp = New<Tests::BezierTests>();
        wm->addSlabWindow(temp);

        // Lento:
        temp = New<Tests::FourierTestWindow>();
        // wm->addSlabWindow(temp);

        // temp = New<WindowTreeBuilderTest>();
        // wm->addSlabWindow(temp);

        // temp = New<WindowPanelTest>();
        // wm->addSlabWindow(temp);

        // temp = New<GLFreeTypeTests>();
        // wm->addSlabWindow(temp);

        //temp = New<Tests::ImGuiNodesTest1>();
        //wm->addSlabWindow(temp);

        temp = New<Tests::Blueprints::ImGuiNodesTest2>();
        wm->addSlabWindow(temp);

        temp = New<Tests::ImGuiNodesTest3>();
        wm->addSlabWindow(temp);

        backend = Slab::Graphics::GetGraphicsBackend();

        backend->GetMainSystemWindow()->addAndOwnEventListener(wm);

    } else {
        Core::BackendManager::Startup("GLFW");
        auto guiBackend = Slab::Graphics::GetGraphicsBackend();
        backend = guiBackend;
        guiBackend->GetMainSystemWindow()->addEventListener(Pointer<Graphics::SystemWindowEventListener>(new Tests::NuklearTests()));
    }

    backend->run();

    return 0;
}
