//
// Created by joao on 10/24/25.
//

#include "App.h"

#include <SFML/Graphics/View.hpp>

#include "Physics/FPlaneFactory.h"
#include "Core/SlabCore.h"
#include "Graphics/OpenGL/LegacyGL/SceneSetup.h"
#include "Graphics/OpenGL/LegacyGL/ShapeRenderer.h"
#include "Graphics/Plot2D/Plotter.h"
#include "Graphics/Plot2D/PlotThemeManager.h"
#include "Math/Function/RtoR/Model/FunctionsCollection/NativeFunction.h"
#include "Physics/Foils/NACA2412.h"
#include "Graphics/Modules/Animator/Animator.h"
#include "Render/PlaneStats.h"

using CAirfoil = Foil::ViternaAirfoil2412;

auto PrettyDraw = true;
auto DebugDraw = true;

constexpr float TimeScale = 1.0f;

constexpr float VectorScale = .00875f;

constexpr auto LightMaterialDensity = 12.0f;
constexpr auto HeavyMaterialDensity = 30.0f;

// constexpr auto InitialViewWidth = 3.2f*1.2f;
constexpr auto InitialViewWidth = 30*1.2f;

constexpr float timeStep = TimeScale/60.0f;
constexpr int subSteps = 1;
constexpr int manualSubSteps = 10;

constexpr auto StartRunning = true;

FLittlePlaneDesignerApp::FLittlePlaneDesignerApp(const int argc, const char* argv[])
    : FApplication("Little Plane Designer", argc, argv)
      , World(), WinHeight(0), WinWidth(0), b_IsRunning(false) {
}

FLittlePlaneDesignerApp::~FLittlePlaneDesignerApp() { b2DestroyWorld(World); }

bool FLittlePlaneDesignerApp::NotifyRender(const Graphics::FPlatformWindow& PlatformWindow)  {
    using namespace Slab;

    namespace Drawer = Graphics::OpenGL::Legacy;
    WinHeight = PlatformWindow.GetHeight();
    WinWidth = PlatformWindow.GetWidth();

    const auto [x, y] = LittlePlane->GetPosition();

    Camera.Update(1.f/60.f);

    fix KeyboardState = PlatformWindow.GetKeyboardState();
    HandleInputs(*KeyboardState);

    // Update writers
    {
        const auto Writer = DebugDraw_LegacyGL->GetWriter();
        Writer->Reshape(WinWidth, WinHeight);
        Writer->SetPenPositionTransform([this](const Graphics::Point2D& PenPosition) {
            IN View = Camera.GetView();
            return Graphics::FromSpaceToViewportCoord(
                PenPosition,
                Graphics::RectR{View.xMin, View.xMax, View.yMin, View.yMax},
                Graphics::RectI{0, WinWidth, 0, WinHeight});
        });
    }

    Graphics::OpenGL::SetViewport(Graphics::RectI{0, WinWidth, 0, WinHeight});
    Drawer::ResetModelView();
    IN View = Camera.GetView();
    Drawer::SetupOrtho({View.xMin, View.xMax, View.yMin, View.yMax});

    if      (PrettyDraw) PlatformWindow.Clear(Graphics::LapisLazuli);
    else if (DebugDraw)  PlatformWindow.Clear(Graphics::Black);

    if constexpr (false) GUIContext->AddDrawCall([this] {
        ImGui::Begin("Wings");
        ImGui::Checkbox("Run", &b_IsRunning);
        ImGui::End();
    });

    UpdateGraphs();

    PlaneStats->Draw(PlatformWindow);

    if (PrettyDraw) {
        Drawer::SetupLegacyGL();

        Terrain->Draw(PlatformWindow);

        LittlePlane->Draw(PlatformWindow);
    }
    if (DebugDraw) { DoDebugDraw(); }

    RenderSimData(PlatformWindow);

    return true;
}

bool FLittlePlaneDesignerApp::NotifyKeyboard(Graphics::EKeyMap key, Graphics::EKeyState state,
                                             Graphics::EModKeys modKeys) {
    if (state == Graphics::EKeyState::Press)
    {
        if (modKeys.Mod_Alt) {
            if (key == Graphics::EKeyMap::Key_F4) {
                GetPlatform()->GetMainSystemWindow()->SignalClose();
                return true;
            }
        }

        if (key == Graphics::EKeyMap::Key_F5) {
            PrettyDraw = !PrettyDraw;
            return true;
        }

        if (key == Graphics::EKeyMap::Key_F6) {
            DebugDraw = !DebugDraw;
            return true;
        }

        if (key == Graphics::EKeyMap::Key_SPACE) b_IsRunning = !b_IsRunning;
    }
    return false;
}

void FLittlePlaneDesignerApp::SetupMonitors() {
    const auto BaseYPosition = Graphics::WindowStyle::GlobalMenuHeight + 20;
    const auto YDelta = 20 + PlotsHeight;

    auto PlotRegion = [](const DevFloat xw, const DevFloat yw)
    {
        return Graphics::PlottingRegion2D(Graphics::RectR{DegToRad(-xw), DegToRad(xw), -yw, yw});
    };

    using Plotter = Graphics::FPlotter;

    const auto C_l   = New<Math::RtoR::NativeFunction>([this](const DevFloat AoA) { return LittlePlane->GetWing(0).Airfoil->Cl(AoA); });
    const auto C_d   = New<Math::RtoR::NativeFunction>([this](const DevFloat AoA) { return LittlePlane->GetWing(0).Airfoil->Cd(AoA); });
    const auto C_mc4 = New<Math::RtoR::NativeFunction>([this](const DevFloat AoA) { return LittlePlane->GetWing(0).Airfoil->Cm_c4(AoA); });

    {
        fix n = Plots.size();
        const auto Ref = Plots.emplace_back(New<Graphics::FPlot2DWindow>("Force polars"));
        Ref->Set_x(20);
        Ref->Set_y(n*YDelta + BaseYPosition);
        Ref->SetNoGUI();
        Ref->SetRegion(PlotRegion(180, 3.0));

        auto Style = Graphics::PlotThemeManager::GetCurrent()->FuncPlotStyles[0];
        Style.lineColor = Graphics::LightGrey;
        Plotter::AddRtoRFunction(Ref, C_l, Style, "C_l");

        Style = Graphics::PlotThemeManager::GetCurrent()->FuncPlotStyles[1];
        Style.lineColor = Graphics::DarkGrass;
        Plotter::AddRtoRFunction(Ref, C_d, Style, "C_d");

        Style.setPrimitive(Graphics::PlottingVerticalLinesWithCircles);
        Style.thickness = 2.0f;
        Style.lineColor = Graphics::White;
        Plotter::AddPointSet(Ref, CurrentLiftPolar, Style, "", false);
        Style.lineColor = Graphics::GrassGreen;
        Plotter::AddPointSet(Ref, CurrentDragPolar, Style, "", false);
    }

    {
        fix n = Plots.size();
        const auto Ref = Plots.emplace_back(New<Graphics::FPlot2DWindow>("Torque polars"));
        Ref->Set_x(20);
        Ref->Set_y(n*YDelta + BaseYPosition);
        Ref->SetNoGUI();
        Ref->TieRegion_xMaxMin(*Plots[0]);
        Ref->GetRegion().set_y_limits(-.08, .08);

        auto Style = Graphics::PlotThemeManager::GetCurrent()->FuncPlotStyles[0];
        Style.lineColor = Graphics::LightGrey;
        Plotter::AddRtoRFunction(Plots[1], C_mc4, Style, "C_mc4");

        Style.setPrimitive(Graphics::PlottingVerticalLinesWithCircles);
        Style.thickness = 2.0f;
        Style.lineColor = Graphics::White;
        Plotter::AddPointSet(Plots[1], CurrentTorquePolar, Style, "", false);

        const auto Theme = Graphics::PlotThemeManager::GetCurrent();
        const TPointer<Graphics::OpenGL::FWriterOpenGL> Writer = Slab::New<Graphics::OpenGL::FWriterOpenGL>(Core::Resources::GetIndexedFontFileName(3), 24);
        Theme->graphBackground.a = 0.25f;
        Theme->LabelsWriter = Writer;
        Theme->TicksWriter = Writer;
    }

    if constexpr (false)
    {
        fix n = Plots.size();

        auto Ref = Plots.emplace_back(New<Graphics::FPlot2DWindow>("Telemetry"));
        Ref->Set_x(20);
        Ref->Set_y(n*YDelta + BaseYPosition);
        Ref->SetNoGUI();
        Ref->SetAutoReviewGraphRanges(true);
        Ref->GetAxisArtist().setVerticalAxisLabel("|F|");
        Ref->GetAxisArtist().SetHorizontalAxisLabel("t");

        auto Style = Graphics::PlotThemeManager::GetCurrent()->FuncPlotStyles[3];
        Plotter::AddPointSet(Plots[2], ForcesTimeSeries, Style, "Forces");
    }

    {
        fix n = Plots.size();

        auto Ref = Plots.emplace_back(New<Graphics::FPlot2DWindow>("Energy"));
        Ref->Set_x(20);
        Ref->Set_y(n*YDelta + BaseYPosition);
        Ref->SetNoGUI();
        Ref->SetAutoReviewGraphRanges(true);
        Ref->GetAxisArtist().setVerticalAxisLabel("E");
        Ref->GetAxisArtist().SetHorizontalAxisLabel("t");

        auto Style = Graphics::PlotThemeManager::GetCurrent()->FuncPlotStyles[0];
        Style.lineColor = Graphics::White;
        Plotter::AddPointSet(Ref, EnergyTotalTimeSeries, Style, "E");
        Style.lineColor = Graphics::GrassGreen;
        Plotter::AddPointSet(Ref, EnergyPotentialTimeSeries, Style, "U");
        Style.lineColor = Graphics::Red;
        Plotter::AddPointSet(Ref, EnergyKineticLinearTimeSeries, Style, "K_l");
        Style.lineColor = Graphics::FColor(1, 0, 1, 1);
        Plotter::AddPointSet(Ref, EnergyKineticAngularTimeSeries, Style, "K_a");
    }
}

void FLittlePlaneDesignerApp::SetupPlane() {
    // Plane
    LittlePlane = FPlaneFactory{}
                  .SetPosition({35.0f, 18.f})
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
                      .xOffset = -1.5,

                  })
                  .AddWing(FWingDescriptor{
                      .Density = LightMaterialDensity,
                      .Airfoil = New<Foil::ViternaAirfoil2412>(),
                      .Params = Foil::FAirfoilParams{
                          .Name = "Wing",
                          .ChordLength = 1.0f,
                          .Span = 6.0f,
                      },
                      .RelativeLocation = {-1, 0.0f},
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
                      .RelativeLocation = {+1.75, 0.1f},
                      .BaseAngle = static_cast<float>(DegToRad(0.0)),
                      .MaxAngle  = static_cast<float>(DegToRad(15)),
                      .MinAngle  = static_cast<float>(DegToRad(-15)),
                      .OscFreq = 10.0f,
                      .DampRatio = 1.0f,
                  })
                  .BuildPlane(World);
}

void FLittlePlaneDesignerApp::SetupTerrain() {
    Terrain = Slab::New<FTerrain>();
    Terrain->Setup(World, FTerrainDescriptor{
                       .H = [](const float t) {
                           const Vector<double> A = {0.75, 0.25, 0.1};
                           constexpr auto BaseFreq = 0.1*M_PI;

                           if (t < 0.0f) {
                               auto Value = 0.0f;
                               int n=0;
                               for (fix a : A)
                                   Value += a*sinf(n++*BaseFreq*t);

                               return Value;

                           }

                           return 0.25f*t;
                       },
                       .x = [](const float t) {
                           if (t < 0.0f) {
                               return 2.0f * t;
                           }

                           return t;
                       },
                       .tMin = -500.0f,
                       .tMax = 50.0f,
                       .Count = 550,
                   });

    b2
}

void FLittlePlaneDesignerApp::OnStart() {
    Core::GetModule("ImGui");
    const auto SystemWindow = Graphics::GetGraphicsBackend()->GetMainSystemWindow();
    SystemWindow->SetupGUIContext();
    GUIContext = SystemWindow->GetGUIContext();

    DebugDraw_LegacyGL = Slab::New<LegacyGLDebugDraw>(VectorScale);

    b_IsRunning = StartRunning;

    // World
    b2WorldDef worldDef = b2DefaultWorldDef();
    worldDef.gravity = (b2Vec2){0.0f, -9.8f};
    World = b2CreateWorld(&worldDef);

    SetupTerrain();

    SetupPlane();

    SetupMonitors();

    // Setup stats
    PlaneStats = New<FPlaneStats>(LittlePlane, World);


    WinWidth = SystemWindow->GetWidth();
    WinHeight = SystemWindow->GetHeight();
    fix AspectRatio = static_cast<float>(WinWidth) / WinHeight;
    Camera.SetParams_Width(InitialViewWidth);
    Camera.SetParams_Ratio(AspectRatio);
    Camera.TrackObject(LittlePlane);
    const auto [x, y] = LittlePlane->GetPosition();
    Camera.SetCenter({x-7.0f, y});
}

void FLittlePlaneDesignerApp::HandleInputs(const Graphics::FKeyboardState& KeyboardState) {
    if (KeyboardState.IsPressed(Graphics::Key_MINUS)) {
        Camera.Zoom(1.01f);
    } if (KeyboardState.IsPressed(Graphics::Key_EQUAL)) {
        Camera.Zoom(.99f);
    }

    if (KeyboardState.IsPressed(Graphics::Key_LEFT_SHIFT)) {
        fix PanDelta = Camera.GetView().GetWidth()*.005f;
        if (KeyboardState.IsPressed(Graphics::Key_LEFT))    Camera.Pan({-PanDelta, 0.0f});
        if (KeyboardState.IsPressed(Graphics::Key_RIGHT))   Camera.Pan({ PanDelta, 0.0f});
        if (KeyboardState.IsPressed(Graphics::Key_UP))      Camera.Pan({0.0f,  PanDelta});
        if (KeyboardState.IsPressed(Graphics::Key_DOWN))    Camera.Pan({0.0f, -PanDelta});
    }
    else {
        bool Flaps = false;
        const auto Wing = LittlePlane->GetWing(0);
        const auto Δα = DegToRad(0.25f);
        if (KeyboardState.IsPressed(Graphics::Key_UP)) {
            LittlePlane->AdjustWingAngle(0, Δα);
        }
        if (KeyboardState.IsPressed(Graphics::Key_DOWN)) {
            LittlePlane->AdjustWingAngle(0, -Δα);
        }
        if (KeyboardState.IsPressed(Graphics::Key_LEFT)) {
            fix RevJoint = Wing.RevJoint;
            fix WingBody = Wing.BodyId;

            b2RevoluteJoint_SetTargetAngle(RevJoint, Wing.MinAngle);
            b2Body_SetAwake(WingBody, true);
            Flaps = true;
        }
        if (KeyboardState.IsPressed(Graphics::Key_RIGHT)) {
            fix RevJoint = Wing.RevJoint;
            fix WingBody = Wing.BodyId;

            b2RevoluteJoint_SetTargetAngle(RevJoint, Wing.MaxAngle);
            b2Body_SetAwake(WingBody, true);
            Flaps = !Flaps; // little trick, figure why
        }
        if (!Flaps) {
            fix RevJoint = Wing.RevJoint;
            b2RevoluteJoint_SetTargetAngle(RevJoint, Wing.BaseAngle);
        }

    }

}

void FLittlePlaneDesignerApp::UpdateGraphs() const {
    const auto &Wing = LittlePlane->GetWing(0);

    const auto CurrentForces = FLittlePlane::ComputeForces(Wing);
    CurrentLiftPolar->clear();
    CurrentLiftPolar->AddPoint(CurrentForces.AoA, CurrentForces.Cl);
    CurrentDragPolar->clear();
    CurrentDragPolar->AddPoint(CurrentForces.AoA, CurrentForces.Cd);
    CurrentTorquePolar->clear();
    CurrentTorquePolar->AddPoint(CurrentForces.AoA, CurrentForces.Cm_c4);
    if (b_IsRunning) {
        StepSimulation();

        static double time = 0.0;
        time += timeStep;

        auto TotalForcesMag = b2Length(CurrentForces.GetTotalForce());
        ForcesTimeSeries->AddPoint({time, TotalForcesMag});

        const auto &WingBody = Wing.BodyId;
        fix v = b2Length(b2Body_GetLinearVelocity(WingBody));
        fix w = b2Body_GetAngularVelocity(WingBody);
        fix h = b2Body_GetPosition(WingBody).y;
        fix m = b2Body_GetMass(WingBody);
        fix I = b2Body_GetRotationalInertia(WingBody);
        fix g = b2Length(b2World_GetGravity(World));
        fix K_l = .5f*m*v*v;
        fix K_a = .5f*I*w*w;
        fix U = m*g*h;
        fix E = U + K_l + K_a;

        EnergyTotalTimeSeries           ->AddPoint({time, E});
        EnergyPotentialTimeSeries       ->AddPoint({time, U});
        EnergyKineticLinearTimeSeries   ->AddPoint({time, K_l});
        EnergyKineticAngularTimeSeries  ->AddPoint({time, K_a});
    }
}

void FLittlePlaneDesignerApp::DoDebugDraw() const {
    auto &Drawer = *DebugDraw_LegacyGL->handle();
    Drawer.drawMass = false;
    Drawer.drawBounds = false;
    Drawer.drawIslands = false;
    Drawer.drawBodyNames = false;
    Drawer.drawShapes = true;
    Drawer.drawJoints = false;

    DebugDraw_LegacyGL->SetupLegacyGL();
    b2World_Draw(World, &Drawer);

    const auto Mass = LittlePlane->GetTotalMass();
    const auto COM = LittlePlane->GetCenterOfMass_Global();

    const auto Grav = b2World_GetGravity(World);
    DebugDraw_LegacyGL->DrawVector(Mass * Grav, COM, 1.0f, b2_colorForestGreen);
}

void FLittlePlaneDesignerApp::RenderSimData(const Graphics::FPlatformWindow& PlatformWindow) {
    const auto w = GetPlatform()->GetMainSystemWindow()->GetWidth();
    const auto h = GetPlatform()->GetMainSystemWindow()->GetHeight();

    for (const auto &Plot : Plots) {
        Plot->NotifySystemWindowReshape(w, h);
        Plot->NotifyReshape(600, PlotsHeight);
        Plot->NotifyRender(PlatformWindow);
    }
}

void FLittlePlaneDesignerApp::StepSimulation() const {
    constexpr float splitTimeStep = timeStep/manualSubSteps;
    for (int i = 0; i < manualSubSteps; ++i) {
        LittlePlane->ComputeAndApplyForces(DebugDraw&&i==0 ? DebugDraw_LegacyGL : nullptr);
        b2World_Step(World, splitTimeStep, subSteps);
    }
}
