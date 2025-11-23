//
// Created by joao on 11/20/25.
//

#include "FLPDPhysicsEngine.h"

#include "Graphics/Utils.h"

constexpr int SubSteps = 1;
constexpr int ManualSubSteps = 10;
constexpr float VectorScale = 1.e-3f;

FLittlePlaneDesignerPhysicsEngine::FLittlePlaneDesignerPhysicsEngine() {
    auto worldDef = b2DefaultWorldDef();
    worldDef.gravity = (b2Vec2){0.0f, -9.8f};
    World = b2CreateWorld(&worldDef);
}

void FLittlePlaneDesignerPhysicsEngine::Tick(const Seconds ElapsedTime) {
    if (!b_IsRunning) return;

    const auto SplitTimeStep = ElapsedTime/ManualSubSteps;
    for (int i = 0; i < ManualSubSteps; ++i) {
        if (Plane) Plane->ComputeAndApplyForces();
        b2World_Step(World, SplitTimeStep, SubSteps);
    }
}

void FLittlePlaneDesignerPhysicsEngine::TogglePause() {
    b_IsRunning = !b_IsRunning;
}

void FLittlePlaneDesignerPhysicsEngine::ToggleDraw() { b_DrawPhysics = !b_DrawPhysics; }

void FLittlePlaneDesignerPhysicsEngine::Draw(const Graphics::FDraw2DParams & Params) {
    if (!b_DrawPhysics) return;

    DebugDraw.GetWriter()->Reshape(Params.ScreenWidth, Params.ScreenHeight);
    DebugDraw.GetWriter()->SetPenPositionTransform([&](const Graphics::Point2D Pt) {
        return FromSpaceToViewportCoord(Pt, Params.Region, Graphics::RectI{0, Params.ScreenWidth, 0, Params.ScreenHeight});
    });

    auto &Drawer = *DebugDraw.handle();
    Drawer.drawMass = true;
    Drawer.drawBounds = false;
    Drawer.drawIslands = false;
    Drawer.drawBodyNames = false;
    Drawer.drawShapes = true;
    Drawer.drawJoints = false;

    DebugDraw.SetupLegacyGL();

    b2World_Draw(World, &Drawer);

    constexpr auto Scale = VectorScale;

    const auto Mass = Plane->GetTotalMass();
    const auto COM =  Plane->GetCenterOfMass_Global();

    const auto Grav = b2World_GetGravity(World);
    DebugDraw.DrawVector(Mass * Grav, COM, Scale, b2_colorForestGreen);

    {
        for (const auto ForcesData : Plane->GetLastAirfoilDynamicData()) {
            const auto drag = ForcesData.drag;
            const auto lift = ForcesData.lift;
            const auto loc = ForcesData.loc;
            const auto τ = ForcesData.torque;

            DebugDraw.DrawVector(drag, loc, Scale, b2_colorRed);
            DebugDraw.Write("drag", loc + drag*Scale, b2_colorRed);

            DebugDraw.DrawVector(lift, loc, Scale, b2_colorAliceBlue);
            DebugDraw.Write("lift", loc + lift*Scale, b2_colorAliceBlue);

            DebugDraw.DrawPseudoVector(τ, loc, Scale, .0f, b2_colorDarkCyan);
            DebugDraw.Write("Torque (aero)", loc + b2Vec2{static_cast<float>(τ)*Scale, .0f}, b2_colorDarkCyan);
        }
    }

    const auto [vx, vy] = Plane->GetVelocity();
    fix VertSpeed = b2Vec2{0, vy};
    DebugDraw.DrawVector(VertSpeed, COM, 1.0f, b2_colorWhite);
    DebugDraw.Write("Vert speed", COM + VertSpeed);
}

