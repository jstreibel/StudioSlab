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
constexpr b2Vec2 v0{-1.f, 0.0f};


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

        const auto AirfoilForces = Foil::ComputeAirfoilForces(Airfoil, WingBody, AeroParams, *DebugDraw_LegacyGL);
        // const auto AirfoilForces = Foil::ComputeAirfoilForces2(Airfoil, WingBody, AeroParams, *DebugDraw_LegacyGL);
        // const auto AirfoilForces =  Foil::FAirfoilForces::Null();
        // Apply at c/4
        b2Body_ApplyForce (WingBody, AirfoilForces.GetTotalForce(), AirfoilForces.loc, true);
        b2Body_ApplyTorque(WingBody, AirfoilForces.torque, true);

        constexpr float timeStep = TimeScale/60.0f;
        constexpr int subSteps = 60;
        b2World_Step(world, timeStep, subSteps);

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
                    fix chord = AeroParams.ChordLength;
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

    ~LittlePlaneDesigner() override {
        b2DestroyWorld(world);
    }

protected:
    b2WorldId world;
    b2BodyId WingBody;
    Foil::FAeroParams AeroParams;
    TPointer<Graphics::FPlot2DWindow> PlotWindow;

    static void SetBodyCOM(const float xCOM, const b2BodyId Body)
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

        // 4) Aero reference consistent with geometry
        AeroParams.ChordLength = Chord;
        AeroParams.span  = 0.10f;
        AeroParams.rho   = 1.225;
        AeroParams.LE_local = (b2Vec2){ -0.25f*Chord, 0.0f };   // LE in local frame

        SetBodyCOM(-0.25f*Chord, WingBody);
    }

    void OnStart() override
    {
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

        PlotWindow = New<Graphics::FPlot2DWindow>("Polars");
        PlotWindow->Set_x(20);
        PlotWindow->Set_y(20);
        PlotWindow->SetNoGUI();
        PlotWindow->SetRegion(Graphics::PlottingRegion2D(Graphics::RectR{DegToRad(-15), DegToRad(15), -1.7, 1.7}));
        auto Polar = New<Math::RtoR::NativeFunction>([this](DevFloat AoA) {
            return Airfoil.Cl(AoA);
        });
        auto Style = Graphics::PlotThemeManager::GetCurrent()->FuncPlotStyles[0];
        Style.lineColor = Graphics::White;
        Graphics::FPlotter::AddRtoRFunction(PlotWindow, Polar, Style, "C_l");

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

        PlotWindow->NotifySystemWindowReshape(w, h);
        PlotWindow->NotifyReshape(600, 400);
        PlotWindow->NotifyRender(PlatformWindow);
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