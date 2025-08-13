//
// Created by joao on 29/08/2021.
//

#include "TestsApp.h"

#include "AppBase.h"
#include "Core/Controller/CommandLine/CommandLineArgsManager.h"
#include "Graphics/Window/SlabWindowManager.h"

#include "BezierTests.h"
#include "WindowPanelTest.h"
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
#include "Graphics/ImGui/ImGuiWindowManager.h"
#include "Graphics/SFML/Graph.h"

#define DONT_REGISTER false

TestsApp:: TestsApp(int argc, const char**argv) : Slab::Core::AppBase(argc, argv, DONT_REGISTER) {

    AppBase::RegisterToManager();

    Slab::Core::CLArgsManager::Parse(argc, argv);
}


int TestsApp::run() {
    using namespace Slab;

    TPointer<Graphics::GraphicBackend> Backend = nullptr;

    if(true) {
        Core::BackendManager::Startup("GLFW");

        Backend = Slab::Graphics::GetGraphicsBackend();
        auto MainSystemWindow = Backend->GetMainSystemWindow();
        // auto wm = New<Graphics::SlabWindowManager>();
        auto ImGuiContext = DynamicPointerCast<Graphics::FImGuiContext>(MainSystemWindow->GetGUIContext());
        auto WindowManager = New<Graphics::FImGuiWindowManager>(ImGuiContext);
        MainSystemWindow->AddAndOwnEventListener(WindowManager);

        WindowManager->AddSlabWindow(New<Tests::LaTeXTests>(), false);
        WindowManager->AddSlabWindow(New<Tests::VShapeExpansionTest>(), false);
        WindowManager->AddSlabWindow(New<Tests::ModernGLTests>(), false);
        WindowManager->AddSlabWindow(New<Tests::Graph3DTests>(), false);
        WindowManager->AddSlabWindow(New<Tests::FBezierTests>(ImGuiContext), false);
        WindowManager->AddSlabWindow(New<Tests::FourierTestWindow>(), false);
        WindowManager->AddSlabWindow(Tests::GetImGuiTestWindow(), false);

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
        Backend = guiBackend;
        guiBackend->GetMainSystemWindow()->AddEventListener(TPointer<Graphics::FPlatformWindowEventListener>(new Tests::NuklearTests()));
    }

    Backend->Run();

    return 0;
}
