//
// Created by joao on 29/08/2021.
//

#include "TestsApp.h"

#include "AppBase.h"
#include "Core/Controller/CommandLine/CommandLineArgsManager.h"
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
#include "ImGuiUITest.h"

#define DONT_REGISTER false

TestsApp:: TestsApp(int argc, const char**argv) : Slab::Core::AppBase(argc, argv, DONT_REGISTER) {

    AppBase::RegisterToManager();

    Slab::Core::CLArgsManager::Parse(argc, argv);
}


int TestsApp::run() {
    using namespace Slab;

    Pointer<Graphics::GraphicBackend> backend = nullptr;

    if(true) {
        Core::BackendManager::Startup("GLFW");

        backend = Slab::Graphics::GetGraphicsBackend();
        auto main_syswin = backend->GetMainSystemWindow();
        auto wm = New<Graphics::SlabWindowManager>();
        main_syswin->addAndOwnEventListener(wm);

        wm->addSlabWindow(New<Tests::LaTeXTests>());
        wm->addSlabWindow(New<Tests::VShapeExpansionTest>());
        wm->addSlabWindow(New<Tests::ModernGLTests>());
        wm->addSlabWindow(New<Tests::Graph3DTests>());
        wm->addSlabWindow(New<Tests::BezierTests>());
        wm->addSlabWindow(New<Tests::FourierTestWindow>());
        wm->addSlabWindow(Tests::GetImGuiTestWindow());

        // temp = New<WindowTreeBuilderTest>();
        // wm->addSlabWindow(temp);

        // temp = New<WindowPanelTest>();
        // wm->addSlabWindow(temp);

        // temp = New<GLFreeTypeTests>();
        // wm->addSlabWindow(temp);

        // temp = New<Tests::ImGuiNodesTest1>();
        // wm->addSlabWindow(temp);

        // temp = New<Tests::ImGuiNodesTest3>();
        // wm->addSlabWindow(temp);

    } else {
        Core::BackendManager::Startup("GLFW");
        auto guiBackend = Slab::Graphics::GetGraphicsBackend();
        backend = guiBackend;
        guiBackend->GetMainSystemWindow()->addEventListener(Pointer<Graphics::SystemWindowEventListener>(new Tests::NuklearTests()));
    }

    backend->Run();

    return 0;
}
