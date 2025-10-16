//
// Created by joao on 8/14/25.
//

#include "Application.h"
#include "StudioSlab.h"
#include "Graphics/SlabGraphics.h"
#include "Graphics/OpenGL/LegacyGL/PointSetRenderer.h"
#include "Graphics/SFML/Graph.h"

#include "box2d/box2d.h"
#include "Core/Tools/Log.h"
#include "Graphics/OpenGL/LegacyGL/SceneSetup.h"
#include "Graphics/OpenGL/LegacyGL/ShapeRenderer.h"

class StudioSlabApp final : public Slab::FApplication {
public:
    bool NotifyRender(const Slab::Graphics::FPlatformWindow& PlatformWindow) override
    {
        using namespace Slab;

        const Graphics::PlotStyle Wing{Graphics::White, Graphics::Quads};

        PlatformWindow.Clear(Graphics::LapisLazuli);

        const Math::PointSet_ptr Points = Slab::New<Math::PointSet>();
        Points->AddPoint(-0.5, 1);
        Points->AddPoint(0.5, 1);
        Points->AddPoint(0.5, 0);
        Points->AddPoint(-0.5, 0);

        namespace Drawer = Graphics::OpenGL::Legacy;
        fix WinHeight = PlatformWindow.GetHeight();
        fix WinWidth = PlatformWindow.GetWidth();

        fix AspectRatio = static_cast<float>(WinWidth) / WinHeight;

        fix ViewSize = 40.0;
        fix ViewHeight = ViewSize / AspectRatio;

        Drawer::ResetModelView();
        Drawer::SetupOrtho({-ViewSize*.5, ViewSize*.5, -.1*ViewHeight, .9*ViewHeight});
        Drawer::RenderPointSet(Points, Wing);

        Drawer::SetColor(Graphics::DarkGrass);
        Drawer::DrawRectangle({Graphics::Point2D{-100, 0}, Graphics::Point2D{100, -5}});
        Drawer::DrawLine({-100, 0}, {100, 0}, Graphics::GrassGreen);

        return true;
    }

    StudioSlabApp(const int argc, const char* argv[]) : FApplication("Little Plane Designer", argc, argv) { }

protected:
    void OnStart() override
    {
        // World
        b2WorldDef worldDef = b2DefaultWorldDef();
        worldDef.gravity = (b2Vec2){0.0f, -10.0f};
        b2WorldId world = b2CreateWorld(&worldDef);

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
        bodyDef.position = (b2Vec2){0.0f, 4.0f};
        b2BodyId body = b2CreateBody(world, &bodyDef);

        b2Polygon box = b2MakeBox(1.0f, 1.0f);
        b2ShapeDef shapeDef = b2DefaultShapeDef();
        shapeDef.density = 1.0f;
        shapeDef.material.friction = 0.3f;
        b2CreatePolygonShape(body, &shapeDef, &box);

        // Step loop
        constexpr float timeStep = 1.0f/60.0f;
        for (int i = 0; i < 90; ++i) {
            constexpr int subSteps = 4;
            b2World_Step(world, timeStep, subSteps);
            b2Vec2 p = b2Body_GetPosition(body);
            b2Rot r = b2Body_GetRotation(body);
            printf("%2d: x=%.2f y=%.2f angle=%.2f\n", i, p.x, p.y, b2Rot_GetAngle(r));
        }

        b2DestroyWorld(world);
    }
};

int main(const int argc, const char *argv[])
{
    return Slab::Run<StudioSlabApp>(argc, argv);
}