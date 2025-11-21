//
// Created by joao on 10/24/25.
//

#include "App.h"

#include "Core/SlabCore.h"
#include "Graphics/OpenGL/LegacyGL/ShapeRenderer.h"
#include "Graphics/Plot2D/Plotter.h"
#include "Graphics/Plot2D/PlotThemeManager.h"
#include "Graphics/Modules/Animator/Animator.h"
#include "Physics/Materials.h"
#include "Physics/Foils/NACA2412.h"
#include "Plane/FPlaneController.h"
#include "Plane/FPlaneFactory.h"
#include "Render/Drawables.h"
#include "Render/PlaneStats.h"
#include "Scenes/BigHill.h"

#include  "Scenes/Scene.h"
#include "Utils/Angles.h"

FLittlePlaneDesignerApp::FLittlePlaneDesignerApp(const int argc, const char* argv[])
: FApplication("Little Plane Designer", argc, argv) { }

bool FLittlePlaneDesignerApp::NotifyRender(const Graphics::FPlatformWindow& PlatformWindow)  {
    using namespace Slab;

    fix &MouseState = *PlatformWindow.GetMouseState();
    fix &KeyboardState = *PlatformWindow.GetKeyboardState();

    PlatformWindow.Clear(Graphics::Black);

    Controller->HandleInputState({KeyboardState, MouseState});

    constexpr auto TimeStepMilisseconds = 1000.0f / 60.0f;
    Camera.Tick(TimeStepMilisseconds);
    PhysicsEngine.Tick(TimeStepMilisseconds);
    CurrentScene->Tick(TimeStepMilisseconds);

    const auto Params = Graphics::FDraw2DParams {
        .ScreenWidth = PlatformWindow.GetWidth(),
        .ScreenHeight = PlatformWindow.GetHeight(),
        .Region = Camera.GetView()
    };

    for (auto &Drawable : Drawables) Drawable->Draw(Params);

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

        if (key == Graphics::EKeyMap::Key_SPACE) CurrentScene->TogglePause();
    }
    return false;
}


void FLittlePlaneDesignerApp::OnStart() {
    Core::LoadModule("ModernOpenGL");
    Core::GetModule("ImGui");

    const auto SystemWindow = Graphics::GetGraphicsBackend()->GetMainSystemWindow();
    SystemWindow->SetupGUIContext();
    const auto WinWidth = SystemWindow->GetWidth();
    const auto WinHeight = SystemWindow->GetHeight();

    PhysicsEngine = FLittlePlaneDesignerPhysicsEngine{};

    auto World = PhysicsEngine.GetWorld();

    const auto BigHill = Slab::New<FBigHill>(World);

    CurrentScene = BigHill;
    CurrentScene->Startup(*SystemWindow);

    Plane = SetupPlane(World);
    PhysicsEngine.SetPlane(Plane);

    Camera.SetParams_BaseWidth(20.0f);
    fix AspectRatio = static_cast<float>(WinWidth) / WinHeight;
    Camera.SetParams_Ratio(AspectRatio);
    Camera.TrackObject(Plane);
    const auto [x, y] = Plane->GetPosition();
    Camera.SetCenter({x-7.0f, y});
    Camera.TrackObject(Plane);

    Controller = Slab::New<FPlaneController>(Plane);

    Drawables.emplace_back(Slab::New<FSky>());
    Drawables.emplace_back(Plane);
    Drawables.emplace_back(BigHill);
}

TPointer<FLittlePlane> FLittlePlaneDesignerApp::SetupPlane(const b2WorldId World) {

    auto PlaneFactory = FPlaneFactory{}
    .SetPosition({161.5f, 41.f})
    .SetRotation(DegToRad(0.0f))
    .AddBodyPart({
        .Density = LightMaterialDensity,
        .Width = 4.0f,
        .Height = 0.5,
    })
    .AddBodyPart({
        .Density = HeavyMaterialDensity,
        .Width = 1,
        .Height = 0.25,
        .xOffset = -1.2,
    })
    .AddBodyPart({
        .Density = LightMaterialDensity,
        .Width = 0.4,
        .Height = 0.4,
        .xOffset = -1.0,
        .yOffset = -0.5,
    })
    .AddWing(FWingDescriptor{
        .Density = LightMaterialDensity,
        .Airfoil = New<Foil::ViternaAirfoil2412>(),
        .Params = Foil::FAirfoilParams{
            .Name = "Wing",
            .ChordLength = 1.15f,
            .Span = 6.0f,
        },
        .RelativeLocation = {-.6f, -0.1f},
        .BaseAngle = static_cast<float>(DegToRad(0.0)),
        .MaxAngle  = static_cast<float>(DegToRad(15)),
        .MinAngle  = static_cast<float>(DegToRad(-15)),
    })
    .AddWing(FWingDescriptor{
        .Density = LightMaterialDensity,
        .Airfoil = New<Foil::ViternaAirfoil2412>(),
        .Params = Foil::FAirfoilParams{
            .Name = "Winglet",
            .ChordLength = 0.6f,
            .Span = 2.f,
        },
        .RelativeLocation = {+1.4, 0.0f},
        .BaseAngle = static_cast<float>(DegToRad(0.0)),
        .MaxAngle  = static_cast<float>(DegToRad(15)),
        .MinAngle  = static_cast<float>(DegToRad(-15)),
        .OscFreq = 10.0f,
        .DampRatio = 1.0f,
    });

    return PlaneFactory.BuildPlane(World);
}
