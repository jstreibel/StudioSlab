//
// Created by joao on 29/08/2021.
//

#include "TestsApp.h"

#include "AppBase.h"
#include "../../Lib/Core/Controller/CommandLine/CommandLineArgsManager.h"
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
#include "Graphics/SlabGraphics.h"
#include "Graphics/ImGui/ImGuiWindowManager.h"
#include "Graphics/SFML/Graph.h"

#define DONT_REGISTER false

FTestsApp::FTestsApp(int argc, const char**argv) : Slab::Core::FAppBase(argc, argv, DONT_REGISTER) {

    FAppBase::RegisterToManager();

    Slab::Core::FCLArgsManager::Parse(argc, argv);
}


int FTestsApp::run() {
    using namespace Slab;

    TPointer<Graphics::FGraphicBackend> Backend = nullptr;

    if(true) {
        Core::FBackendManager::Startup("GLFW");

        Backend = Slab::Graphics::GetGraphicsBackend();
        auto MainSystemWindow = Backend->GetMainSystemWindow();
        // auto wm = New<Graphics::SlabWindowManager>();
        auto ImGuiContext = DynamicPointerCast<Graphics::FImGuiContext>(MainSystemWindow->SetupGUIContext());
        auto WindowManager = New<Graphics::FImGuiWindowManager>(ImGuiContext);
        MainSystemWindow->AddAndOwnEventListener(WindowManager);

        WindowManager->AddSlabWindow(New<Tests::FLaTeXTests>(), false);
        WindowManager->AddSlabWindow(New<Tests::FVShapeExpansionTest>(), false);
        WindowManager->AddSlabWindow(New<Tests::FModernGLTests>(), false);
        WindowManager->AddSlabWindow(New<Tests::FGraph3DTests>(), false);
        WindowManager->AddSlabWindow(New<Tests::FBezierTests>(), false);
        WindowManager->AddSlabWindow(New<Tests::FFourierTestWindow>(), false);
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
        Core::FBackendManager::Startup("GLFW");
        auto guiBackend = Slab::Graphics::GetGraphicsBackend();
        Backend = guiBackend;
        guiBackend->GetMainSystemWindow()->AddEventListener(TPointer<Graphics::FPlatformWindowEventListener>(new Tests::FNuklearTests()));
    }

    Backend->Run();

    return 0;
}
