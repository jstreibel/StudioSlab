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

class LittlePlaneDesigner final : public FApplication {
public:
    bool NotifyRender(const Graphics::FPlatformWindow& PlatformWindow) override
    {
        using namespace Slab;

        namespace Drawer = Graphics::OpenGL::Legacy;
        fix WinHeight = PlatformWindow.GetHeight();
        fix WinWidth = PlatformWindow.GetWidth();
        fix AspectRatio = static_cast<float>(WinWidth) / WinHeight;
        fix ViewHeight = ViewSize / AspectRatio;
        Drawer::ResetModelView();
        Drawer::SetupOrtho({-ViewSize*.5, ViewSize*.5, -.1*ViewHeight, .9*ViewHeight});

        PlatformWindow.Clear(Graphics::LapisLazuli);

        const Graphics::PlotStyle Wing{Graphics::White, Graphics::LineLoop};

        const Math::PointSet AirfoilPoints = Airfoil.GetProfileVertices(8);
        Math::PointSet Points = AirfoilPoints;

        const auto AirfoilForces = Foil::ComputeAirfoilForces(Airfoil, WingBody, P);
        // Apply at c/4
        b2Body_ApplyForce (WingBody, AirfoilForces.GetTotalForce(), AirfoilForces.loc, true);
        b2Body_ApplyTorque(WingBody, AirfoilForces.torque, true);

        constexpr float timeStep = 1.0f/(2.f*60.0f);
        constexpr int subSteps = 60;
        b2World_Step(world, timeStep, subSteps);
        const auto [x, y] = b2Body_GetPosition(WingBody);
        const auto [c, s] = b2Body_GetRotation(WingBody);

        if constexpr (false) {
            Drawer::SetColor(Graphics::DarkGrass);
            Drawer::DrawRectangle({Graphics::Point2D{-100, 0}, Graphics::Point2D{100, -5}});
            Drawer::DrawLine({-100, 0}, {100, 0}, Graphics::GrassGreen);

            for (auto &Point : Points.getPoints()) {
                fix px = Point.x;
                fix py = Point.y;
                Point.x = x + px*c - py*s;
                Point.y = y + px*s + py*c;
            }
            Drawer::RenderPointSet(Dummy(Points), Wing);
        } else {
            DebugDraw(AirfoilForces);
        }

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
    Foil::Airfoil_NACA2412 Airfoil;
    const float Chord = 1.0f;
    const float Thick = 0.10f;

    float ViewSize = 10.0;

    b2WorldId world;
    b2BodyId WingBody;
    Foil::FAeroParams P;

    const float BoxHalfWidth = 0.5f;
    const float BoxHalfHeight = 0.06f;

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
        bodyDef.position = (b2Vec2){ViewSize*.0f, ViewSize*.25f};
        bodyDef.rotation = b2MakeRot(0.0123f);
        bodyDef.angularVelocity = 0.8354123f;
        bodyDef.linearVelocity = (b2Vec2){-1.f, 0.0f};
        WingBody = b2CreateBody(world, &bodyDef);
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
        sdef.material.friction = 0.5f;
        b2CreatePolygonShape(WingBody, &sdef, &WingShape);

        // 4) Aero reference consistent with geometry
        P.ChordLength = Chord;
        P.span  = 0.10f;
        P.rho   = 1.225;
        P.LE_local = (b2Vec2){ -0.25f*Chord, 0.0f };   // LE in local frame

        SetBodyCOM(-0.25f*Chord, WingBody);
    }

    void OnStart() override
    {
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
    }

    void DebugDraw(const Foil::FAirfoilForces &AirfoilForces) const {
        static LegacyGLDebugDraw DebugDraw_LegacyGL;
        auto &Drawer = *DebugDraw_LegacyGL.handle();
        // Drawer.drawMass = false;
        Drawer.drawBounds = false;
        Drawer.drawIslands = false;

        b2World_Draw(world, &Drawer);

        // DebugDraw_LegacyGL.DrawForce(AirfoilForces.lift, AirfoilForces.loc, 1.0f, b2_colorCoral);
        // DebugDraw_LegacyGL.DrawForce(AirfoilForces.drag, AirfoilForces.loc, 1.0f, b2_colorDarkRed);
        // DebugDraw_LegacyGL.DrawTorque(WingBody, AirfoilForces.torque, 0.05f, 24);
    }
};

int main(const int argc, const char *argv[])
{
    return Slab::Run<LittlePlaneDesigner>(argc, argv);
}