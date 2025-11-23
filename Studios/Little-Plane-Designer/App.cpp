//
// Created by joao on 10/24/25.
//

#include "App.h"

#include "Core/SlabCore.h"
#include "Graphics/OpenGL/LegacyGL/ShapeRenderer.h"
#include "Graphics/Plot2D/Plotter.h"
#include "Graphics/Plot2D/PlotThemeManager.h"
#include "Graphics/Modules/Animator/Animator.h"
#include "Physics/Foils/NACA2412.h"
#include "Plane/FPlaneController.h"
#include "Render/Drawables.h"
#include "Render/PlaneStats.h"
#include "Scenes/BigHill.h"

#include  "Scenes/Scene.h"

FLittlePlaneDesignerApp::FLittlePlaneDesignerApp(const int argc, const char* argv[])
: FApplication("Little Plane Designer", argc, argv) { }

bool FLittlePlaneDesignerApp::NotifyRender(const Graphics::FPlatformWindow& PlatformWindow)  {
    using namespace Slab;

    fix &MouseState = *PlatformWindow.GetMouseState();
    fix &KeyboardState = *PlatformWindow.GetKeyboardState();

    PlatformWindow.Clear(Graphics::Black);

    Controller->HandleInputState({KeyboardState, MouseState});

    CurrentScene->Tick(1.f/60.f);
    CurrentScene->Draw({PlatformWindow});

    return true;
}

bool FLittlePlaneDesignerApp::NotifyKeyboard(
    const Graphics::EKeyMap key,
    const Graphics::EKeyState state,
    const Graphics::EModKeys modKeys)
{
    if (state == Graphics::EKeyState::Press)
    {
        if (modKeys.Mod_Alt) {
            if (key == Graphics::EKeyMap::Key_F4) {
                GetPlatform()->GetMainSystemWindow()->SignalClose();
                return true;
            }
        }

        if (key == Graphics::EKeyMap::Key_F5) { SetSceneBigHill(); return true; }
        if (key == Graphics::EKeyMap::Key_F6) { SetSceneBlueprint(); return true; }

        if (key == Graphics::EKeyMap::Key_SPACE) { CurrentScene->TogglePause(); return true; }
    }

    return Controller->NotifyKeyboard(key, state, modKeys);
}


void FLittlePlaneDesignerApp::OnStart() {
    Core::LoadModule("ModernOpenGL");
    Core::GetModule("ImGui");

    const auto SystemWindow = Graphics::GetGraphicsBackend()->GetMainSystemWindow();
    SystemWindow->SetupGUIContext();

    BlueprintScene = Slab::New<FLittlePlaneBlueprint>();
    auto PlaneFactory = BlueprintScene->GetPlaneFactory();
    BigHill = Slab::New<FBigHill>(PlaneFactory);



    Controller = BigHill;
    CurrentScene = BigHill;

    CurrentScene->Startup(*SystemWindow);
}

void FLittlePlaneDesignerApp::SetSceneBigHill() {
    CurrentScene = BigHill;
    Controller = BigHill;
}

void FLittlePlaneDesignerApp::SetSceneBlueprint() {
    CurrentScene = BlueprintScene;
    Controller = BlueprintScene;
}
