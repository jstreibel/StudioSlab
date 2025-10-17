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

class LittlePlaneDesigner final : public Slab::FApplication {
public:
    bool NotifyRender(const Slab::Graphics::FPlatformWindow& PlatformWindow) override
    {
        using namespace Slab;

        const Graphics::PlotStyle Wing{Graphics::White, Graphics::Quads};

        PlatformWindow.Clear(Graphics::LapisLazuli);

        const Math::PointSet_ptr Points = Slab::New<Math::PointSet>();
        Points->AddPoint(-BoxHalfSide, +BoxHalfSide);
        Points->AddPoint(+BoxHalfSide, +BoxHalfSide);
        Points->AddPoint(+BoxHalfSide, -BoxHalfSide);
        Points->AddPoint(-BoxHalfSide, -BoxHalfSide);

        namespace Drawer = Graphics::OpenGL::Legacy;
        fix WinHeight = PlatformWindow.GetHeight();
        fix WinWidth = PlatformWindow.GetWidth();

        fix AspectRatio = static_cast<float>(WinWidth) / WinHeight;

        constexpr auto ViewSize = 10.0;
        fix ViewHeight = ViewSize / AspectRatio;

        constexpr float timeStep = 1.0f/120.0f;
        constexpr int subSteps = 2;
        b2World_Step(world, timeStep, subSteps);
        const auto [x, y] = b2Body_GetPosition(body);
        const auto [c, s] = b2Body_GetRotation(body);

        Drawer::ResetModelView();
        Drawer::SetupOrtho({-ViewSize*.5, ViewSize*.5, -.1*ViewHeight, .9*ViewHeight});

        Drawer::SetColor(Graphics::DarkGrass);
        Drawer::DrawRectangle({Graphics::Point2D{-100, 0}, Graphics::Point2D{100, -5}});
        Drawer::DrawLine({-100, 0}, {100, 0}, Graphics::GrassGreen);

        for (auto &Point : Points->getPoints()) {
            fix px = Point.x;
            fix py = Point.y;
            Point.x = x + px*c + py*s;
            Point.y = y + px*s - py*c;
        }
        Drawer::RenderPointSet(Points, Wing);

        return true;
    }

    LittlePlaneDesigner(const int argc, const char* argv[])
    : FApplication("Little Plane Designer", argc, argv)
    , world()
    , body() {    }

    ~LittlePlaneDesigner() override {
        b2DestroyWorld(world);
    }

protected:
    b2WorldId world;
    b2BodyId body;
    const float BoxHalfSide = 0.5f;

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
        bodyDef.position = (b2Vec2){0.0f, 5.0f};
        bodyDef.rotation = b2MakeRot(0.38f*M_PI);
        body = b2CreateBody(world, &bodyDef);

        b2Polygon box = b2MakeBox(BoxHalfSide, BoxHalfSide);
        b2ShapeDef shapeDef = b2DefaultShapeDef();
        shapeDef.density = 1.0f;
        shapeDef.material.friction = 0.3f;
        b2CreatePolygonShape(body, &shapeDef, &box);
    }
};

int main(const int argc, const char *argv[])
{
    return Slab::Run<LittlePlaneDesigner>(argc, argv);
}