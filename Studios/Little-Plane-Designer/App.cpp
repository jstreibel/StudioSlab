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

using CAirfoil = Foil::ViternaAirfoil2412;

constexpr auto PrettyDraw = true;
constexpr auto DebugDraw = false;

constexpr float TimeScale = 1;

// constexpr auto InitialViewWidth = 3.2f*1.2f;
constexpr auto InitialViewWidth = 30*1.2f;

constexpr float timeStep = TimeScale/60.0f;
constexpr int subSteps = 1;
constexpr int manualSubSteps = 10;

constexpr auto StartRunning = true;

void FLittlePlaneDesignerApp::ComputeAndApplyForces() const {
    for (auto &Wing : LittlePlane->Wings) {
        const auto &WingBody = Wing->BodyId;
        if (!b2Body_IsAwake(WingBody)) continue;

        const auto CurrentForces = FLittlePlane::ComputeForces(*Wing, {}, DebugDraw ? DebugDraw_LegacyGL : nullptr);
        b2Body_ApplyForce (WingBody, CurrentForces.GetTotalForce(), CurrentForces.loc, true);
        b2Body_ApplyTorque(WingBody, CurrentForces.torque, true);
    }
}

void FLittlePlaneDesignerApp::StepSimulation() const {
    constexpr float splitTimeStep = timeStep/manualSubSteps;
    for (int i = 0; i < manualSubSteps; ++i)
        b2World_Step(World, splitTimeStep, subSteps);
}

void FLittlePlaneDesignerApp::HandleInputs(const Graphics::FKeyboardState& KeyboardState) {
    const auto AspectRatio = static_cast<float>(WinWidth) / WinHeight;

    if (KeyboardState.IsPressed(Graphics::Key_MINUS)) {
        const auto ViewWidth = View.GetWidth() * 1.01f;
        const auto ViewHeight = View.GetWidth() / AspectRatio;

        auto xCenter = View.xCenter();
        View.xMin = xCenter - ViewWidth*.5f;
        View.xMax = xCenter + ViewWidth*.5f;
        View.yMin = - ViewHeight*.1f;
        View.yMax = + ViewHeight*.9f;
    } if (KeyboardState.IsPressed(Graphics::Key_EQUAL)) {
        const auto ViewWidth = View.GetWidth() / 1.01f;
        const auto ViewHeight = View.GetWidth() / AspectRatio;

        auto xCenter = View.xCenter();
        View.xMin = xCenter - ViewWidth*.5f;
        View.xMax = xCenter + ViewWidth*.5f;
        View.yMin = - ViewHeight*.1f;
        View.yMax = + ViewHeight*.9f;
    }

    bool Flaps = false;
    const auto Wing = LittlePlane->Wings[0];
    if (KeyboardState.IsPressed(Graphics::Key_LEFT)) {
        if (KeyboardState.IsPressed(Graphics::Key_LEFT_SHIFT)) {
            fix dWidth = View.GetWidth()*.005f;
            View.xMin -= dWidth;
            View.xMax -= dWidth;
        } else {
            fix RevJoint = Wing->RevJoint;
            fix WingBody = Wing->BodyId;

            b2RevoluteJoint_SetTargetAngle(RevJoint, Wing->MinAngle);
            b2Body_SetAwake(WingBody, true);
            Flaps = true;
        }
    }
    if (KeyboardState.IsPressed(Graphics::Key_RIGHT)) {
        if (KeyboardState.IsPressed(Graphics::Key_LEFT_SHIFT)) {
            fix dWidth = View.GetWidth()*.005f;
            View.xMin += dWidth;
            View.xMax += dWidth;
        } else {
            fix RevJoint = Wing->RevJoint;
            fix WingBody = Wing->BodyId;

            b2RevoluteJoint_SetTargetAngle(RevJoint, Wing->MaxAngle);
            b2Body_SetAwake(WingBody, true);
            Flaps = !Flaps; // little trick, figure why
        }
    }
    if (!Flaps) {
        fix RevJoint = Wing->RevJoint;
        b2RevoluteJoint_SetTargetAngle(RevJoint, Wing->BaseAngle);
    }

}

bool FLittlePlaneDesignerApp::NotifyRender(const Graphics::FPlatformWindow& PlatformWindow)  {
    using namespace Slab;

    namespace Drawer = Graphics::OpenGL::Legacy;
    WinHeight = PlatformWindow.GetHeight();
    WinWidth = PlatformWindow.GetWidth();

    fix KeyboardState = PlatformWindow.GetKeyboardState();
    HandleInputs(*KeyboardState);

    Graphics::OpenGL::SetViewport(Graphics::RectI{0, WinWidth, 0, WinHeight});
    Drawer::ResetModelView();
    Drawer::SetupOrtho({View.xMin, View.xMax, View.yMin, View.yMax});
    const auto Writer = DebugDraw_LegacyGL->GetWriter();
    Writer->Reshape(WinWidth, WinHeight);
    Writer->SetPenPositionTransform([this](const Graphics::Point2D& pt) {
        return Graphics::FromSpaceToViewportCoord(pt,
            Graphics::RectR{View.xMin, View.xMax, View.yMin, View.yMax},
            Graphics::RectI{0, WinWidth, 0, WinHeight});
    });

    PlatformWindow.Clear(Graphics::LapisLazuli);

    if constexpr (false) GUIContext->AddDrawCall([this] {
        ImGui::Begin("Wings");
        ImGui::Checkbox("Run", &b_IsRunning);

        if (LittlePlane == nullptr)
            ImGui::TextColored({1,0,0,1}, "No plane");
        else for (const auto &Wing : LittlePlane->Wings) {

            ImGui::SeparatorText(Str(Wing->Params.Name).c_str());

            const auto WingBody = Wing->BodyId;

            const auto foil_vel = b2Body_GetLinearVelocity(WingBody);
            float foil_speed = b2Length(foil_vel);
            float foil_angle = atan2f(foil_vel.y, -foil_vel.x);
            float foil_angular_speed = b2Body_GetAngularVelocity(WingBody);
            if (ImGui::SliderFloat("COM speed mag", &foil_speed, 0.0f, 10.0f)
                | ImGui::SliderAngle("Speed angle", &foil_angle, -180.0f, 180.0f))
            {

                b2Body_SetLinearVelocity(WingBody, {-foil_speed*cosf(foil_angle), foil_speed*sinf(foil_angle)});
            }
            if (ImGui::DragFloat("Angular speed", &foil_angular_speed, fabs(foil_angular_speed)*1.e-2f+1.e-2, -720.0f, 720.0f)) {
                b2Body_SetAngularVelocity(WingBody, foil_angular_speed);
            }
        }

        ImGui::End();
    });

    {
        const auto &Wing = LittlePlane->Wings[0];

        const auto CurrentForces = FLittlePlane::ComputeForces(*Wing);
        CurrentLiftPolar->clear();
        CurrentLiftPolar->AddPoint(CurrentForces.AoA, CurrentForces.Cl);
        CurrentDragPolar->clear();
        CurrentDragPolar->AddPoint(CurrentForces.AoA, CurrentForces.Cd);
        CurrentTorquePolar->clear();
        CurrentTorquePolar->AddPoint(CurrentForces.AoA, CurrentForces.Cm_c4);
        if (b_IsRunning) {
            ComputeAndApplyForces();

            StepSimulation();

            static double time = 0.0;
            time += timeStep;

            auto TotalForcesMag = b2Length(CurrentForces.GetTotalForce());
            ForcesTimeSeries->AddPoint({time, TotalForcesMag});

            const auto &WingBody = Wing->BodyId;
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

    if constexpr (PrettyDraw) {
        Drawer::SetupLegacyGL();

        /*
        Drawer::SetColor(Graphics::DarkGrass);
        Drawer::DrawRectangle({Graphics::Point2D{-100, 0}, Graphics::Point2D{100, -5}});
        Drawer::DrawLine({-100, 0}, {100, 0}, Graphics::GrassGreen, 3.f);
        */

        Terrain->Draw();

        LittlePlane->Draw();
    }
    if constexpr (DebugDraw) { DoDebugDraw(); }

    Monitor(PlatformWindow);

    return true;
}

void FLittlePlaneDesignerApp::SetupMonitors() {
    const auto BaseYPosition = Graphics::WindowStyle::GlobalMenuHeight + 20;
    const auto YDelta = 20 + PlotsHeight;

    auto PlotRegion = [](const DevFloat xw, const DevFloat yw)
    {
        return Graphics::PlottingRegion2D(Graphics::RectR{DegToRad(-xw), DegToRad(xw), -yw, yw});
    };

    using Plotter = Graphics::FPlotter;

    const auto C_l   = New<Math::RtoR::NativeFunction>([this](const DevFloat AoA) { return LittlePlane->Wings[0]->Airfoil->Cl(AoA); });
    const auto C_d   = New<Math::RtoR::NativeFunction>([this](const DevFloat AoA) { return LittlePlane->Wings[0]->Airfoil->Cd(AoA); });
    const auto C_mc4 = New<Math::RtoR::NativeFunction>([this](const DevFloat AoA) { return LittlePlane->Wings[0]->Airfoil->Cm_c4(AoA); });

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

    if constexpr (0)
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

void FLittlePlaneDesignerApp::OnStart() {
    Core::GetModule("ImGui");
    const auto SystemWindow = Graphics::GetGraphicsBackend()->GetMainSystemWindow();
    SystemWindow->SetupGUIContext();
    GUIContext = SystemWindow->GetGUIContext();

    DebugDraw_LegacyGL = Slab::New<LegacyGLDebugDraw>();

    WinWidth = SystemWindow->GetWidth();
    WinHeight = SystemWindow->GetHeight();
    fix AspectRatio = static_cast<float>(WinWidth) / WinHeight;
    fix InitialViewHeight = InitialViewWidth / AspectRatio;
    View = {
        -InitialViewWidth*.5f,
         InitialViewWidth*.5f,
        -.1f*InitialViewHeight,
         .9f*InitialViewHeight};

    b_IsRunning = StartRunning;

    // World
    b2WorldDef worldDef = b2DefaultWorldDef();
    worldDef.gravity = (b2Vec2){0.0f, -9.8f};
    World = b2CreateWorld(&worldDef);

    // Ground body
    b2BodyDef groundDef = b2DefaultBodyDef();                 // static by default
    groundDef.position = (b2Vec2){0.0f, -10.0f};
    const b2BodyId ground = b2CreateBody(World, &groundDef);
    const b2Polygon groundBox = b2MakeBox(50.0f, 10.0f);
    const b2ShapeDef groundShapeDef = b2DefaultShapeDef();
    b2CreatePolygonShape(ground, &groundShapeDef, &groundBox);

    // Ramp
    if constexpr (false) {
        b2BodyDef RampDef = b2DefaultBodyDef();
        RampDef.position = {8.0f, 4.0f};
        RampDef.fixedRotation = true;
        RampDef.rotation = b2MakeRot(DegToRad(15.0f));
        const b2BodyId Ramp = b2CreateBody(World, &RampDef);
        const b2Polygon RampBox = b2MakeBox(10.0f, 0.25f);
        const b2ShapeDef BoxShape = b2DefaultShapeDef();
        b2CreatePolygonShape(Ramp, &BoxShape, &RampBox);
    }

    // Terrain
    Terrain = Slab::New<FTerrain>();
    Terrain->Setup(World, FTerrainDescriptor{
        .H = [](const float x) {
            if (x < 0.0f) return 0.0f;

            return 0.25f*x;
        },
        .tMin = -50.0f,
        .tMax = 50.0f,
        .Count = 100,
    });

    // Plane
    FPlaneFactory Factory{};

    LittlePlane = Factory
    .SetPosition({12.0f, 7.f})
    // .SetPosition({0.f, 0.21f})
    .SetRotation(DegToRad(0.0f))
    .SetBodyDensity(0.01f)
    .AddWing(FWingDescriptor{
        .Airfoil = New<Foil::ViternaAirfoil2412>(),
        .Params = Foil::FAirfoilParams{
            .Name = "Wing",
            // .Span = 6.0f
            .ChordLength = 1.0f
        },
        .RelativeLocation = {-1, 0.0f},
        .BaseAngle = static_cast<float>(DegToRad(0.0)),
        .MaxAngle  = static_cast<float>(DegToRad(15)),
        .MinAngle  = static_cast<float>(DegToRad(-15))
    })
    .AddWing(FWingDescriptor{
        .Airfoil = New<Foil::ViternaAirfoil2412>(),
        .Params = Foil::FAirfoilParams{
            .Name = "Winglet",
            // .Span = 2.f
            .ChordLength = 0.4f
        },
        .RelativeLocation = {+0.5, 0.1f},
        .BaseAngle = static_cast<float>(DegToRad(2.0)),
        .MaxAngle  = static_cast<float>(DegToRad(15)),
        .MinAngle  = static_cast<float>(DegToRad(-15)),
        .OscFreq = 10.0f,
        .DampRatio = 1.0f,
    })
    .BuildPlane(World);

    SetupMonitors();
}

void FLittlePlaneDesignerApp::DoDebugDraw() const {
    auto &Drawer = *DebugDraw_LegacyGL->handle();
    Drawer.drawMass = true;
    Drawer.drawBounds = false;
    Drawer.drawIslands = false;
    Drawer.drawBodyNames = true;
    Drawer.drawShapes = true;

    DebugDraw_LegacyGL->SetupLegacyGL();
    b2World_Draw(World, &Drawer);

    // DebugDraw_LegacyGL.DrawForce(AirfoilForces.lift, AirfoilForces.loc, 1.0f, b2_colorCoral);
    // DebugDraw_LegacyGL.DrawForce(AirfoilForces.drag, AirfoilForces.loc, 1.0f, b2_colorDarkRed);
    // DebugDraw_LegacyGL.DrawTorque(WingBody, AirfoilForces.torque, 0.05f, 24);
}

void FLittlePlaneDesignerApp::Monitor(const Graphics::FPlatformWindow& PlatformWindow) {
    const auto w = GetPlatform()->GetMainSystemWindow()->GetWidth();
    const auto h = GetPlatform()->GetMainSystemWindow()->GetHeight();

    for (const auto &Plot : Plots) {
        Plot->NotifySystemWindowReshape(w, h);
        Plot->NotifyReshape(600, PlotsHeight);
        Plot->NotifyRender(PlatformWindow);
    }
}

bool FLittlePlaneDesignerApp::NotifyKeyboard(Graphics::EKeyMap key, Graphics::EKeyState state,
    Graphics::EModKeys modKeys) {
    if (state == Graphics::EKeyState::Press)
    {
        if (key == Graphics::EKeyMap::Key_F4 && modKeys.Mod_Alt) {
            GetPlatform()->GetMainSystemWindow()->SignalClose();
            return true;
        }

        if (key == Graphics::EKeyMap::Key_SPACE) b_IsRunning = !b_IsRunning;

        if (key == Graphics::EKeyMap::Key_MINUS) {
            // Graphics::Animator::Set(ViewWidth, ViewWidth*1.2, 0.1);
        } else if (key == Graphics::EKeyMap::Key_EQUAL) {
            // Graphics::Animator::Set(ViewWidth, ViewWidth/1.2, 0.1);
        }
    }
    return false;
}

FLittlePlaneDesignerApp::FLittlePlaneDesignerApp(const int argc, const char* argv[])
    : FApplication("Little Plane Designer", argc, argv)
      , World(), View(), WinHeight(0), WinWidth(0), b_IsRunning(false) {
}

FLittlePlaneDesignerApp::~FLittlePlaneDesignerApp() { b2DestroyWorld(World); }