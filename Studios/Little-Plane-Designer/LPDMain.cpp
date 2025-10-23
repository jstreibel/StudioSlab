//
// Created by joao on 8/14/25.
//

#include "Application.h"
#include "DebugDraw.h"
#include "Foil.h"
#include "NACA2412.h"
#include "StudioSlab.h"
#include "Graphics/SlabGraphics.h"
#include "Graphics/OpenGL/LegacyGL/PointSetRenderer.h"
#include "Graphics/SFML/Graph.h"

#include "box2d/box2d.h"
#include "Core/SlabCore.h"
#include "Core/Tools/Log.h"
#include "Graphics/OpenGL/LegacyGL/SceneSetup.h"
#include "Graphics/OpenGL/LegacyGL/ShapeRenderer.h"
#include "Graphics/Plot2D/Plot2DWindow.h"
#include "Graphics/Plot2D/Plotter.h"
#include "Graphics/Plot2D/PlotThemeManager.h"
#include "Math/Function/RtoR/Model/FunctionsCollection/NativeFunction.h"

#define DeclarePointset(Name) TPointer<Math::PointSet> Name = New<Math::PointSet>();

constexpr auto PrettyDraw = true;
constexpr auto DebugDraw = true;

constexpr float TimeScale = 0.1f;

constexpr b2Vec2 x0{2.5f, 1.5f};
constexpr auto α0 = .0f; // 0.3f;
constexpr auto ω0 = .0f; // 0.8354123f;
constexpr b2Vec2 v0{-5.f, 0.0f};

auto DegToRad(const auto ang) { return ang * M_PI / 180.0;}
auto RadToDeg(const auto ang) { return ang * 180.0 / M_PI;}

using CAirfoil = Foil::ViternaAirfoil2412;

class LittlePlaneDesigner final : public FApplication {
    TPointer<LegacyGLDebugDraw> DebugDraw_LegacyGL;

    CAirfoil Airfoil;
    const float Chord = 1.0f;
    const float Thick = 0.10f;

    float ViewWidth = 7.0;

    bool b_IsRunning = false;

public:
    bool NotifyRender(const Graphics::FPlatformWindow& PlatformWindow) override
    {
        using namespace Slab;

        namespace Drawer = Graphics::OpenGL::Legacy;
        fix WinHeight = PlatformWindow.GetHeight();
        fix WinWidth = PlatformWindow.GetWidth();
        fix AspectRatio = static_cast<float>(WinWidth) / WinHeight;
        fix ViewHeight = ViewWidth / AspectRatio;

        Graphics::OpenGL::SetViewport(Graphics::RectI{0, WinWidth, 0, WinHeight});
        Drawer::ResetModelView();
        Drawer::SetupOrtho({-ViewWidth*.5, ViewWidth*.5, -.1*ViewHeight, .9*ViewHeight});
        const auto Writer = DebugDraw_LegacyGL->GetWriter();
        Writer->Reshape(WinWidth, WinHeight);
        Writer->SetPenPositionTransform([this, ViewHeight, WinWidth, WinHeight](const Graphics::Point2D& pt) {
            return Graphics::FromSpaceToViewportCoord(pt,
                Graphics::RectR{-.5f*ViewWidth, +.5f*ViewWidth, -.1f*ViewHeight, .9f*ViewHeight },
                Graphics::RectI{0, WinWidth, 0, WinHeight});
        });

        PlatformWindow.Clear(Graphics::LapisLazuli);

        GUIContext->AddDrawCall([this] {
            ImGui::Begin("Wing");
            ImGui::Checkbox("Run", &b_IsRunning);

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

            ImGui::End();
        });

        const auto CurrentForces = Foil::ComputeAirfoilForces(Airfoil, WingBody, *DebugDraw_LegacyGL);
        CurrentLiftPolar->clear();
        CurrentLiftPolar->AddPoint(CurrentForces.AoA, CurrentForces.Cl);
        CurrentDragPolar->clear();
        CurrentDragPolar->AddPoint(CurrentForces.AoA, CurrentForces.Cd);
        CurrentTorquePolar->clear();
        CurrentTorquePolar->AddPoint(CurrentForces.AoA, CurrentForces.Cm_c4);
        if (b_IsRunning) {
            // Apply at c/4
            b2Body_ApplyForce (WingBody, CurrentForces.GetTotalForce(), CurrentForces.loc, true);
            b2Body_ApplyTorque(WingBody, CurrentForces.torque, true);

            static double time = 0.0;
            constexpr float timeStep = TimeScale/60.0f;
            time += timeStep;
            constexpr int subSteps = 60;
            b2World_Step(world, timeStep, subSteps);

            auto TotalForcesMag = b2Length(CurrentForces.GetTotalForce());
            ForcesTimeSeries->AddPoint({time, TotalForcesMag});

            fix v = b2Length(b2Body_GetLinearVelocity(WingBody));
            fix w = b2Body_GetAngularVelocity(WingBody);
            fix h = b2Body_GetPosition(WingBody).y;
            fix m = b2Body_GetMass(WingBody);
            fix I = b2Body_GetRotationalInertia(WingBody);
            fix g = b2Length(b2World_GetGravity(world));
            fix K_l = .5f*m*v*v;
            fix K_a = .5f*I*w*w;
            fix U = m*g*h;
            fix E = U + K_l + K_a;

            EnergyTotalTimeSeries           ->AddPoint({time, E});
            EnergyPotentialTimeSeries       ->AddPoint({time, U});
            EnergyKineticLinearTimeSeries   ->AddPoint({time, K_l});
            EnergyKineticAngularTimeSeries  ->AddPoint({time, K_a});
        }

        if constexpr (PrettyDraw) {
            Drawer::SetColor(Graphics::DarkGrass);
            Drawer::DrawRectangle({Graphics::Point2D{-100, 0}, Graphics::Point2D{100, -5}});
            Drawer::DrawLine({-100, 0}, {100, 0}, Graphics::GrassGreen, 3.f);

            {
                Graphics::PlotStyle WingStyle{Graphics::White, Graphics::TriangleFan};
                WingStyle.thickness = 2.0f;
                WingStyle.lineColor.a = 0.5f;
                const Math::PointSet AirfoilPoints = Airfoil.GetProfileVertices(200);
                Math::PointSet Points = AirfoilPoints; // Math::PointSet(Math::Point2DVec{{.25f*Chord ,.0f}}) + AirfoilPoints;
                const auto [x, y] = b2Body_GetPosition(WingBody);
                const auto [c, s] = b2Body_GetRotation(WingBody);
                for (auto &Point : Points.getPoints()) {
                    constexpr auto chord = 1.0; // ;Airfoil.Params.ChordLength;
                    fix px = Point.x-chord*.5f;
                    fix py = Point.y;

                    Point.x = x + px*c - py*s;
                    Point.y = y + px*s + py*c;
                }
                Drawer::RenderPointSet(Dummy(Points), WingStyle);
            }
        }
        if constexpr (DebugDraw) {
            DoDebugDraw();
        }

        Monitor(PlatformWindow);

        return true;
    }

protected:
    b2WorldId world;
    b2BodyId WingBody;

    TPointer<Graphics::FGUIContext> GUIContext;

    static void ShiftBodyCOM(const float Δx, const float Δy, const b2BodyId Body)
    {
        // target COM shift (local body frame)

        // md0.mass, md0.center, md0.I (I about body origin)
        auto [mass, center, rotationalInertia] = b2Body_GetMassData(Body);

        const float m  = mass;
        const auto [x, y] = center;

        // inertia about current COM
        const float I_com = rotationalInertia - m * (x*x + y*y);

        // new local center
        const auto c1 = (b2Vec2){ Δx, Δy };

        // inertia about body origin with new COM
        const float I1 = I_com + m * (c1.x*c1.x + c1.y*c1.y);

        // apply
        const b2MassData md1 = { .mass = m, .center = c1, .rotationalInertia = I1 };
        b2Body_SetMassData(Body, md1);
        b2Body_SetAwake(Body, true);
    }

    void SetupWing()
    {
        // Dynamic box
        b2BodyDef bodyDef = b2DefaultBodyDef();
        bodyDef.type = b2_dynamicBody;
        // bodyDef.position = (b2Vec2){ViewSize*(.5f), ViewSize*.25f};
        bodyDef.position = x0;
        bodyDef.rotation =  b2MakeRot(α0);
        bodyDef.angularVelocity = ω0;
        bodyDef.linearVelocity = v0;
        WingBody = b2CreateBody(world, &bodyDef);
        b2Body_SetName(WingBody, "Wing");
        // Rectangle with c/4 at (0,0):
        // LE=-0.25c, TE=+0.75c

        // 1) Build hull
        b2Hull hull;
        if constexpr (false)
        {
            constexpr int N = B2_MAX_POLYGON_VERTICES;
            const auto AirfoilPoints = Airfoil.GetProfileVertices(N).getPoints();
            b2Vec2 pts[N];
            for (int i=0; i<N; ++i) {
                pts[i].x = AirfoilPoints[i].x;
                pts[i].y = AirfoilPoints[i].y;
            }
            hull = b2ComputeHull(pts, N);
        }
        else
        {
            b2Vec2 pts[4] = {
                { -0.5f*Chord, -0.5f*Thick },
                {  0.5f*Chord, -0.5f*Thick },
                {  0.5f*Chord,  0.5f*Thick },
                { -0.5f*Chord,  0.5f*Thick }
            };
            hull = b2ComputeHull(pts, 4);
        }

        // 2) Make convex polygon from hull
        const b2Polygon WingShape = b2MakePolygon(&hull, 0.0f);          // if your API needs radius: b2MakePolygon(&hull, 0.0f)

        // 3) Create a fixture
        b2ShapeDef sdef = b2DefaultShapeDef();
        sdef.density = 1.0f;
        sdef.material.friction = 0.1f;
        b2CreatePolygonShape(WingBody, &sdef, &WingShape);

        ShiftBodyCOM(-0.25f*Chord, -0.001*Thick, WingBody); // 38% behind LE (NACA2412 usually sits 35-40% chord length)
    }

    const int PlotsHeight = 400;
    Vector<TPointer<Graphics::FPlot2DWindow>> Plots;

    DeclarePointset(ForcesTimeSeries)

    DeclarePointset(EnergyTotalTimeSeries)
    DeclarePointset(EnergyPotentialTimeSeries)
    DeclarePointset(EnergyKineticLinearTimeSeries)
    DeclarePointset(EnergyKineticAngularTimeSeries)

    DeclarePointset(CurrentLiftPolar)
    DeclarePointset(CurrentDragPolar)
    DeclarePointset(CurrentTorquePolar)

    void SetupMonitors() {
        const auto BaseYPosition = Graphics::WindowStyle::GlobalMenuHeight + 20;
        const auto YDelta = 20 + PlotsHeight;

        auto PlotRegion = [](const DevFloat xw, const DevFloat yw)
        {
            return Graphics::PlottingRegion2D(Graphics::RectR{DegToRad(-xw), DegToRad(xw), -yw, yw});
        };

        using Plotter = Graphics::FPlotter;

        const auto C_l   = New<Math::RtoR::NativeFunction>([this](const DevFloat AoA) { return Airfoil.Cl(AoA); });
        const auto C_d   = New<Math::RtoR::NativeFunction>([this](const DevFloat AoA) { return Airfoil.Cd(AoA); });
        const auto C_mc4 = New<Math::RtoR::NativeFunction>([this](const DevFloat AoA) { return Airfoil.Cm_c4(AoA); });

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

    void OnStart() override
    {
        Core::GetModule("ImGui");
        auto SystemWindow = Graphics::GetGraphicsBackend()->GetMainSystemWindow();
        SystemWindow->SetupGUIContext();
        GUIContext = SystemWindow->GetGUIContext();

        DebugDraw_LegacyGL = Slab::New<LegacyGLDebugDraw>();

        // World
        b2WorldDef worldDef = b2DefaultWorldDef();
        worldDef.gravity = (b2Vec2){0.0f, -10.0f};
        world = b2CreateWorld(&worldDef);

        // Ground body
        b2BodyDef groundDef = b2DefaultBodyDef();                 // static by default
        groundDef.position = (b2Vec2){0.0f, -10.0f};
        b2BodyId ground = b2CreateBody(world, &groundDef);
        b2Polygon groundBox = b2MakeBox(50.0f, 10.0f);
        b2ShapeDef groundShapeDef = b2DefaultShapeDef();
        b2CreatePolygonShape(ground, &groundShapeDef, &groundBox);

        SetupWing();

        SetupMonitors();
    }

    void DoDebugDraw() const {
        auto &Drawer = *DebugDraw_LegacyGL->handle();
        Drawer.drawMass = true;
        Drawer.drawBounds = false;
        Drawer.drawIslands = false;
        Drawer.drawBodyNames = false;
        Drawer.drawShapes = true;

        b2World_Draw(world, &Drawer);

        // DebugDraw_LegacyGL.DrawForce(AirfoilForces.lift, AirfoilForces.loc, 1.0f, b2_colorCoral);
        // DebugDraw_LegacyGL.DrawForce(AirfoilForces.drag, AirfoilForces.loc, 1.0f, b2_colorDarkRed);
        // DebugDraw_LegacyGL.DrawTorque(WingBody, AirfoilForces.torque, 0.05f, 24);
    }

    void Monitor(const Graphics::FPlatformWindow& PlatformWindow) {
        const auto w = GetPlatform()->GetMainSystemWindow()->GetWidth();
        const auto h = GetPlatform()->GetMainSystemWindow()->GetHeight();

        for (const auto &Plot : Plots) {
            Plot->NotifySystemWindowReshape(w, h);
            Plot->NotifyReshape(600, PlotsHeight);
            Plot->NotifyRender(PlatformWindow);
        }
    }

public:
    bool NotifyKeyboard(Graphics::EKeyMap key, Graphics::EKeyState state, Graphics::EModKeys modKeys) override {
        if (state == Graphics::EKeyState::Press)
        {
            if (key == Graphics::EKeyMap::Key_F4 && modKeys.Mod_Alt) {
                GetPlatform()->GetMainSystemWindow()->SignalClose();
                return true;
            }

            if (key == Graphics::EKeyMap::Key_SPACE) b_IsRunning = !b_IsRunning;
        }
        return false;
    }

    LittlePlaneDesigner(const int argc, const char* argv[])
    : FApplication("Little Plane Designer", argc, argv)
    , world()
    , WingBody() {    }

    ~LittlePlaneDesigner() override { b2DestroyWorld(world); }


};

int main(const int argc, const char *argv[]) { return Slab::Run<LittlePlaneDesigner>(argc, argv); }