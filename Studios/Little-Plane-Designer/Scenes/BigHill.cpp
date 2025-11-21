//
// Created by joao on 11/20/25.
//

#include "BigHill.h"
#include "../Physics/Foils/NACA2412.h"
#include "Graphics/Plot2D/Plotter.h"

#include "Graphics/OpenGL/LegacyGL/SceneSetup.h"
#include "Graphics/Modules/Animator/Animator.h"
#include "../Render/Drawables.h"
#include "../Render/PlaneStats.h"

struct FPlaneStats;
using CAirfoil = Foil::ViternaAirfoil2412;

auto PrettyDraw = true;
auto DebugDraw = false;

constexpr float TimeScale = 1.0f;

constexpr float VectorScale = 0.005f;

constexpr auto HeightOfADomesticCat = 0.25f;
constexpr auto HeightOfAHuman = 1.80f;

constexpr auto InitialViewWidth = 18*1.2f;

constexpr auto PlotRegionWidth = 15.0f;

constexpr auto StartRunning = true;

void FBigHill::Startup(const Graphics::FPlatformWindow &SystemWindow) {
    GUIContext = SystemWindow.GetGUIContext();

    SetupScenario();

    // DebugDraw_LegacyGL = Slab::New<LegacyGLDebugDraw>();
    // SetupMonitors();
    // PlaneStats = New<FPlaneStats>(LittlePlane, World);
}

void FBigHill::Tick(Miliseconds ElapsedTime) {
    Camera.Tick(1.f/60.f);
}

void FBigHill::Draw(const Graphics::FDraw2DParams& Providers) {
    namespace Drawer = Graphics::OpenGL::Legacy;
    // const auto &PlatformWindow = Providers.ResolutionProvider;

    fix WinHeight = Providers.ScreenHeight;
    fix WinWidth = Providers.ScreenWidth;



    // fix KeyboardState = PlatformWindow.GetKeyboardState();
    // HandleInputs(*KeyboardState);

    // Update writers
    // {
    //     const auto Writer = DebugDraw_LegacyGL->GetWriter();
    //     Writer->Reshape(WinWidth, WinHeight);
    //     Writer->SetPenPositionTransform([Providers](const Graphics::Point2D& PenPosition) {
    //         IN View = Providers.Region;
    //         return Graphics::FromSpaceToViewportCoord(
    //             PenPosition,
    //             Graphics::RectR{View.xMin, View.xMax, View.yMin, View.yMax},
    //             Graphics::RectI{0, Providers.ScreenWidth, 0, Providers.ScreenHeight});
    //     });
    // }

    Graphics::OpenGL::SetViewport(Graphics::RectI{0, WinWidth, 0, WinHeight});
    Drawer::ResetModelView();
    Drawer::SetupOrtho(Providers.Region);

    if constexpr (false) GUIContext->AddDrawCall([this] {
        ImGui::Begin("Wings");
        ImGui::Checkbox("Run", &b_IsRunning);
        ImGui::End();
    });

    if (PrettyDraw) {
        Drawer::SetupLegacyGL();

        for (const auto& Drawable : Drawables) {
            Drawable->Draw(Providers);
        }
    }

    // UpdateGraphs();
    // if (DebugDraw) { DoPhysicsDraw(); }

    /*
    if constexpr (false)
    {
        for (const auto &Plot : Plots) {
            Plot->NotifySystemWindowReshape(WinWidth, WinHeight);
            Plot->NotifyReshape(600, PlotsHeight);
            Plot->NotifyRender(PlatformWindow);
        }
    }
    */

    // PlaneStats->Draw(Providers);
}

void FBigHill::TogglePause() {
    b_IsRunning = !b_IsRunning;
}

/*
void FBigHill::SetupMonitors() {
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
        Ref->SetRegion(PlotRegion(PlotRegionWidth, 3.0));

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
*/

void FBigHill::SetupScenario() {
    constexpr auto Hilltop_x = 160.0f;
    const auto Hilltop = Graphics::Point2D{Hilltop_x, 0.25*Hilltop_x};

    Terrain = Slab::New<FTerrain>();
    Terrain->Setup(World, FTerrainDescriptor
        {
            .H = [](const float t) {
                constexpr auto BaseFreq =  0.1f*static_cast<float>(M_PI);

                struct AOhm { float A; float ohm; };
                auto MultiplyNoise = [](const float Freq) {
                    // return static_cast<float>(Freq * RandUtils::RandomUniformReal(1.005, 0.995));
                    return static_cast<float>(Freq);
                };

                int n=1;
                const auto A = Vector<AOhm>{
                    {0.75f, MultiplyNoise(BaseFreq*n++)},
                    {0.25f, MultiplyNoise(BaseFreq*n++)},
                    {0.10f, MultiplyNoise(BaseFreq*n++)}};

                auto Value = 0.0f;
                for (fix [a, ohm] : A)
                    Value += a*sinf(ohm*t);

                if (t<0) return Value;
                if (t>Hilltop_x) return 0.25f*Value + 0.25f*Hilltop_x;

                return 0.25f*Value + 0.25f*t;
            },
            .x = [](const float t) {
                if (t < 0.0f) return 2.0f * t;
                return t;
            },
            .tMin = -500.0f,
            .tMax = 250.0f,
            .Count = 550,
       });

    constexpr auto ZIndexFront = 0.9f;

    Drawables.emplace_back(Terrain);
    Drawables.emplace_back(Slab::New<FCat>(Hilltop, ZIndexFront));
    Drawables.emplace_back(Slab::New<FRuler>(Hilltop.WithTranslation(-.5, .0), HeightOfADomesticCat, ZIndexFront));
    Drawables.emplace_back(Slab::New<FRuler>(Hilltop.WithTranslation(-2.0, -.8), HeightOfAHuman, ZIndexFront));
    Drawables.emplace_back(Slab::New<FGuy>(Hilltop.WithTranslation(-0.5, 0.9-.8), ZIndexFront));
    Drawables.emplace_back(Slab::New<FTree01>(Hilltop.WithTranslation(+3.0, 1.5), ZIndexFront));
    Drawables.emplace_back(Slab::New<FTree01>(Graphics::Point2D{-5.0, 0.0}, ZIndexFront));
}

/*
void FBigHill::UpdateGraphs() const {

    const auto &Wing = LittlePlane->GetWing(0);

    const auto CurrentForces = FLittlePlane::ComputeForces(Wing);
    CurrentLiftPolar->clear();
    CurrentLiftPolar->AddPoint(CurrentForces.AoA, CurrentForces.Cl);
    CurrentDragPolar->clear();
    CurrentDragPolar->AddPoint(CurrentForces.AoA, CurrentForces.Cd);
    CurrentTorquePolar->clear();
    CurrentTorquePolar->AddPoint(CurrentForces.AoA, CurrentForces.Cm_c4);

    if (b_IsRunning) {
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

void FBigHill::DoPhysicsDraw() const {
    auto &Drawer = *DebugDraw_LegacyGL->handle();
    Drawer.drawMass = false;
    Drawer.drawBounds = false;
    Drawer.drawIslands = false;
    Drawer.drawBodyNames = false;
    Drawer.drawShapes = true;
    Drawer.drawJoints = false;

    DebugDraw_LegacyGL->SetupLegacyGL();

    if constexpr (false) b2World_Draw(World, &Drawer);

    constexpr auto Scale = VectorScale;

    const auto Mass = LittlePlane->GetTotalMass();
    const auto COM = LittlePlane->GetCenterOfMass_Global();

    const auto Grav = b2World_GetGravity(World);
    DebugDraw_LegacyGL->DrawVector(Mass * Grav, COM, Scale, b2_colorForestGreen);

    {
        for (const auto ForcesData : LittlePlane->GetLastAirfoilDynamicData()) {
            const auto drag = ForcesData.drag;
            const auto lift = ForcesData.lift;
            const auto loc = ForcesData.loc;
            const auto τ = ForcesData.torque;

            DebugDraw_LegacyGL->DrawVector(drag, loc, Scale, b2_colorRed);
            DebugDraw_LegacyGL->Write("drag", loc + drag*Scale, b2_colorRed);

            DebugDraw_LegacyGL->DrawVector(lift, loc, Scale, b2_colorAliceBlue);
            DebugDraw_LegacyGL->Write("lift", loc + lift*Scale, b2_colorAliceBlue);

            DebugDraw_LegacyGL->DrawPseudoVector(τ, loc, Scale, .0f, b2_colorDarkCyan);
            DebugDraw_LegacyGL->Write("Torque (aero)", loc + b2Vec2{static_cast<float>(τ)*Scale, .0f}, b2_colorDarkCyan);
        }
    }

    const auto [vx, vy] = LittlePlane->GetVelocity();
    fix VertSpeed = b2Vec2{0, vy};
    DebugDraw_LegacyGL->DrawVector(VertSpeed, COM, 1.0f, b2_colorWhite);
    DebugDraw_LegacyGL->Write("Vert speed", COM + VertSpeed);
}
*/