//
// Created by joao on 8/14/25.
//

#include "Application.h"
#include "DebugDraw.h"
#include "Foil.h"
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

constexpr auto PrettyDraw = true;
constexpr auto DebugDraw = true;

constexpr float TimeScale = 0.1f;

constexpr b2Vec2 x0{4.5f, 5.f};
constexpr auto α0 = .0f; // 0.3f;
constexpr auto ω0 = .0f; // 0.8354123f;
constexpr b2Vec2 v0{-5.f, 0.0f};


inline DevFloat DegToRad(const DevFloat ang) { return ang * M_PI / 180.0;};
inline DevFloat RadToDeg(const DevFloat ang) { return ang * 180.0 / M_PI;};

class LittlePlaneDesigner final : public FApplication {
    TPointer<LegacyGLDebugDraw> DebugDraw_LegacyGL;

    Foil::Airfoil_NACA2412 Airfoil;
    const float Chord = 1.0f;
    const float Thick = 0.10f;

    float ViewWidth = 15.0;

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

        static int options = 1;
        GUIContext->AddDrawCall([this] {
            ImGui::Begin("Wing");
            ImGui::BeginGroup();
            ImGui::RadioButton("Simulate", &options, 0);
            ImGui::RadioButton("Test", &options, 1);
            ImGui::EndGroup();

            if (options == 1) {
                const auto foil_vel = b2Body_GetLinearVelocity(WingBody);
                float foil_speed = b2Length(foil_vel);
                float foil_angle = atan2f(foil_vel.y, foil_vel.x);
                if (ImGui::SliderFloat("Foil speed", &foil_speed, 0.0f, 10.0f)
                  | ImGui::SliderAngle("Foil angle", &foil_angle, -180.0f, 180.0f))
                {
                    b2Body_SetLinearVelocity(WingBody, {foil_speed*cosf(foil_angle), foil_speed*sinf(foil_angle)});
                }
            }

            ImGui::End();
        });

        const auto AirfoilForces = Foil::ComputeAirfoilForces(Airfoil, WingBody, *DebugDraw_LegacyGL);
        if (options == 0) {
            // Apply at c/4
            b2Body_ApplyForce (WingBody, AirfoilForces.GetTotalForce(), AirfoilForces.loc, true);
            b2Body_ApplyTorque(WingBody, AirfoilForces.torque, true);

            constexpr float timeStep = TimeScale/60.0f;
            constexpr int subSteps = 60;
            b2World_Step(world, timeStep, subSteps);
        }

        if constexpr (PrettyDraw) {
            Drawer::SetColor(Graphics::DarkGrass);
            Drawer::DrawRectangle({Graphics::Point2D{-100, 0}, Graphics::Point2D{100, -5}});
            Drawer::DrawLine({-100, 0}, {100, 0}, Graphics::GrassGreen);

            {
                Graphics::PlotStyle WingStyle{Graphics::White, Graphics::TriangleFan};
                WingStyle.thickness = 2.0f;
                WingStyle.lineColor.a = 0.5f;
                Math::PointSet AirfoilPoints = Airfoil.GetProfileVertices(200);
                Math::PointSet Points = AirfoilPoints; // Math::PointSet(Math::Point2DVec{{.25f*Chord ,.0f}}) + AirfoilPoints;
                const auto [x, y] = b2Body_GetPosition(WingBody);
                const auto [c, s] = b2Body_GetRotation(WingBody);
                for (auto &Point : Points.getPoints()) {
                    fix chord = Airfoil.Params.ChordLength;
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

    LittlePlaneDesigner(const int argc, const char* argv[])
    : FApplication("Little Plane Designer", argc, argv)
    , world()
    , WingBody() {    }

    ~LittlePlaneDesigner() override { b2DestroyWorld(world); }

protected:
    b2WorldId world;
    b2BodyId WingBody;
    TPointer<Graphics::FPlot2DWindow> Plots1;
    TPointer<Graphics::FPlot2DWindow> Plots2;

    TPointer<Graphics::FGUIContext> GUIContext;

    static void ShiftBodyCOM(const float xCOM, const b2BodyId Body)
    {
        // target COM shift (local body frame)

        // md0.mass, md0.center, md0.I  (I about body origin)
        auto [mass, center, rotationalInertia] = b2Body_GetMassData(Body);

        const float m  = mass;
        const auto [x, y] = center;

        // inertia about current COM
        const float I_com = rotationalInertia - m * (x*x + y*y);

        // new local center
        const auto c1 = (b2Vec2){ xCOM, y };  // keep y as is (0 for your symmetric hull)

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
        b2Polygon WingShape = b2MakePolygon(&hull, 0.0f);          // if your API needs radius: b2MakePolygon(&hull, 0.0f)

        // 3) Create fixture
        b2ShapeDef sdef = b2DefaultShapeDef();
        sdef.density = 1.0f;
        sdef.material.friction = 0.1f;
        b2CreatePolygonShape(WingBody, &sdef, &WingShape);

        ShiftBodyCOM((-0.5+0.38)*Chord, WingBody); // 38% behind LE (NACA2412 usually sits 35-40% chord length)
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

        auto PlotRegion = [](DevFloat xw, DevFloat yw){return Graphics::PlottingRegion2D(Graphics::RectR{DegToRad(-xw), DegToRad(xw), -yw, yw});};
        Plots1 = New<Graphics::FPlot2DWindow>("Force polars");
        Plots1->Set_x(20);
        Plots1->Set_y(20);
        Plots1->SetNoGUI();
        Plots1->SetRegion(PlotRegion(15, 1.75));

        auto C_l = New<Math::RtoR::NativeFunction>([this](DevFloat AoA) { return Airfoil.Cl(AoA); });
        auto C_d = New<Math::RtoR::NativeFunction>([this](DevFloat AoA) { return Airfoil.Cd(AoA); });
        auto C_mc4 = New<Math::RtoR::NativeFunction>([this](DevFloat AoA) { return Airfoil.Cm_c4(AoA); });

        auto Style = Graphics::PlotThemeManager::GetCurrent()->FuncPlotStyles[0];
        Style.lineColor = Graphics::White;
        Graphics::FPlotter::AddRtoRFunction(Plots1, C_l, Style, "C_l");

        Style = Graphics::PlotThemeManager::GetCurrent()->FuncPlotStyles[1];
        Style.lineColor = Graphics::GrassGreen;
        Graphics::FPlotter::AddRtoRFunction(Plots1, C_d, Style, "C_d");

        Plots2 = New<Graphics::FPlot2DWindow>("Torque polars");
        Plots2->Set_x(20);
        Plots2->Set_y(20 + 20 + 400);
        Plots2->SetNoGUI();
        Plots2->TieRegion_xMaxMin(*Plots1);
        Plots2->GetRegion().set_y_limits(-.08, .08);

        Style = Graphics::PlotThemeManager::GetCurrent()->FuncPlotStyles[0];
        Style.lineColor = Graphics::White;
        Graphics::FPlotter::AddRtoRFunction(Plots2, C_mc4, Style, "C_mc4");


        const auto Theme = Graphics::PlotThemeManager::GetCurrent();
        const TPointer<Graphics::OpenGL::FWriterOpenGL> Writer = Slab::New<Graphics::OpenGL::FWriterOpenGL>(Core::Resources::GetIndexedFontFileName(3), 24);
        Theme->graphBackground.a = 0.25f;
        Theme->LabelsWriter = Writer;
        Theme->TicksWriter = Writer;
    }

    void DoDebugDraw() const {
        auto &Drawer = *DebugDraw_LegacyGL->handle();
        Drawer.drawMass = false;
        Drawer.drawBounds = false;
        Drawer.drawIslands = false;
        Drawer.drawBodyNames = false;
        Drawer.drawShapes = false;

        b2World_Draw(world, &Drawer);

        // DebugDraw_LegacyGL.DrawForce(AirfoilForces.lift, AirfoilForces.loc, 1.0f, b2_colorCoral);
        // DebugDraw_LegacyGL.DrawForce(AirfoilForces.drag, AirfoilForces.loc, 1.0f, b2_colorDarkRed);
        // DebugDraw_LegacyGL.DrawTorque(WingBody, AirfoilForces.torque, 0.05f, 24);
    }

    void Monitor(const Graphics::FPlatformWindow& PlatformWindow) {
        const auto w = GetPlatform()->GetMainSystemWindow()->GetWidth();
        const auto h = GetPlatform()->GetMainSystemWindow()->GetHeight();

        Plots1->NotifySystemWindowReshape(w, h);
        Plots1->NotifyReshape(600, 400);
        Plots1->NotifyRender(PlatformWindow);

        Plots2->NotifySystemWindowReshape(w, h);
        Plots2->NotifyReshape(600, 400);
        Plots2->NotifyRender(PlatformWindow);
    }
public:
    bool NotifyKeyboard(Graphics::EKeyMap key, Graphics::EKeyState state, Graphics::EModKeys modKeys) override {
        if (state == Graphics::EKeyState::Press)
        {
            if (key == Graphics::EKeyMap::Key_F4 && modKeys.Mod_Alt) {
                GetPlatform()->GetMainSystemWindow()->SignalClose();
                return true;
            }
        }
        return false;
    };


};

int main(const int argc, const char *argv[])
{
    return Slab::Run<LittlePlaneDesigner>(argc, argv);
}