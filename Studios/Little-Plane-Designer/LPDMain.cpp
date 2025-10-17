//
// Created by joao on 8/14/25.
//

#include "Application.h"
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

        const Graphics::PlotStyle Wing{Graphics::White, Graphics::LineLoop};

        PlatformWindow.Clear(Graphics::LapisLazuli);

        const Math::PointSet AirfoilPoints = Airfoil.GetProfileVertices();
        Math::PointSet Points = AirfoilPoints;

        namespace Drawer = Graphics::OpenGL::Legacy;
        fix WinHeight = PlatformWindow.GetHeight();
        fix WinWidth = PlatformWindow.GetWidth();

        fix AspectRatio = static_cast<float>(WinWidth) / WinHeight;

        fix ViewHeight = ViewSize / AspectRatio;

        constexpr float timeStep = 1.0f/240.0f;
        constexpr int subSteps = 10;
        Foil::ApplyAirfoilForces(Airfoil, WingBody, P);
        b2World_Step(world, timeStep, subSteps);
        const auto [x, y] = b2Body_GetPosition(WingBody);
        const auto [c, s] = b2Body_GetRotation(WingBody);

        Drawer::ResetModelView();
        Drawer::SetupOrtho({-ViewSize*.5, ViewSize*.5, -.1*ViewHeight, .9*ViewHeight});

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
    float ViewSize = 10.0;

    b2WorldId world;
    b2BodyId WingBody;
    Foil::FAeroParams P;

    const float BoxHalfWidth = 0.5f;
    const float BoxHalfHeight = 0.06f;

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

        // Dynamic box
        b2BodyDef bodyDef = b2DefaultBodyDef();
        bodyDef.type = b2_dynamicBody;
        bodyDef.position = (b2Vec2){ViewSize*.0f, ViewSize*.25f};
        bodyDef.rotation = b2MakeRot(0);
        bodyDef.linearVelocity = (b2Vec2){-1.f, 0.0f};
        WingBody = b2CreateBody(world, &bodyDef);

        // Rectangle with c/4 at (0,0): LE=-0.25c, TE=+0.75c
        constexpr float chord = 1.0f;
        constexpr float thick = 0.05f;

        b2Vec2 pts[4] = {
            { -0.25f*chord, -0.5f*thick },
            {  0.75f*chord, -0.5f*thick },
            {  0.75f*chord,  0.5f*thick },
            { -0.25f*chord,  0.5f*thick }
        };

        // 1) Build hull
        b2Hull hull = b2ComputeHull(pts, 4);

        // 2) Make convex polygon from hull
        b2Polygon WingShape = b2MakePolygon(&hull, 0.0f);          // if your API needs radius: b2MakePolygon(&hull, 0.0f)

        // 3) Create fixture
        b2ShapeDef sdef = b2DefaultShapeDef();
        sdef.density = 1.0f;
        sdef.material.friction = 0.5f;
        b2CreatePolygonShape(WingBody, &sdef, &WingShape);

        // 4) Aero reference consistent with geometry
        P.chord = chord;
        P.span  = 0.10f;
        P.rho   = 1.225;
        P.le_local = (b2Vec2){ -0.25f*chord, 0.0f };   // LE in local frame
    }
};

int main(const int argc, const char *argv[])
{
    return Slab::Run<LittlePlaneDesigner>(argc, argv);
}