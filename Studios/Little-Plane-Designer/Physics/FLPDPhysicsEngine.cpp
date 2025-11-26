//
// Created by joao on 11/20/25.
//

#include "FLPDPhysicsEngine.h"

#include "Graphics/Utils.h"
#include "Utils/OnFirstRun.h"

constexpr int SubSteps = 1;
constexpr int ManualSubSteps = 10;
constexpr float VectorScale = 1.e-3f;
constexpr float PseudoVectorScale = 1.e-2f;

FLittlePlaneDesignerPhysicsEngine::FLittlePlaneDesignerPhysicsEngine() {
    auto worldDef = b2DefaultWorldDef();
    worldDef.gravity = (b2Vec2){0.0f, -9.8f};
    World = b2CreateWorld(&worldDef);

    const auto SystemWindow = Graphics::GetGraphicsBackend()->GetMainSystemWindow();

    auto &Drawer = *DebugDraw.handle();
    Drawer.drawMass = false;
    Drawer.drawShapes = true;
    Drawer.drawJoints = false;
    Drawer.drawIslands = false;
    Drawer.drawBounds = false;
    Drawer.drawBodyNames = false;
    Drawer.drawContacts = false;
    Drawer.drawContactNormals = false;
    Drawer.drawContactImpulses = false;
    Drawer.drawContactFeatures = false;
    Drawer.drawFrictionImpulses = false;
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

    const auto HullMass = Plane->ComputeHullMass();
    const auto WingsMass = Plane->ComputeWingsMass();
    const auto TotalMass = HullMass + WingsMass;
    const auto COM =  Plane->GetCenterOfMass_Global();

    auto &Drawer = *DebugDraw.handle();

    ImGui::Begin("Physics Scheme", nullptr, ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_AlwaysAutoResize);

    static bool b_ShowTorques = true;
    static bool b_ShowAerodynamicForces = true;
    static bool b_ShowGravity = true;
    static bool b_ShowVerticalSpeed = true;

    ImGui::SeparatorText("Aerodynamics");
    ImGui::Checkbox("Torques", &b_ShowTorques);
    ImGui::Checkbox("Aerodynamic forces", &b_ShowAerodynamicForces);
    ImGui::Checkbox("Vertical speed", &b_ShowVerticalSpeed);
    ImGui::SeparatorText("Other physics diagrams");
    ImGui::Checkbox("Gravity", &b_ShowGravity);
    ImGui::Checkbox("Mass", &Drawer.drawMass);
    ImGui::Checkbox("Shapes", &Drawer.drawShapes);
    ImGui::Checkbox("Joints", &Drawer.drawJoints);
    ImGui::Checkbox("Islands", &Drawer.drawIslands);
    ImGui::Checkbox("Bounds", &Drawer.drawBounds);
    ImGui::Checkbox("Body names", &Drawer.drawBodyNames);
    ImGui::Checkbox("Contacts", &Drawer.drawContacts);
    ImGui::Checkbox("Contact normals", &Drawer.drawContactNormals);
    ImGui::Checkbox("Contact impulses", &Drawer.drawContactImpulses);
    ImGui::Checkbox("Contact features", &Drawer.drawContactFeatures);
    ImGui::Checkbox("Friction impulses", &Drawer.drawFrictionImpulses);
    ImGui::SeparatorText("Other");
    ImGui::Checkbox("Running", &b_IsRunning);
    ImGui::Text("Total mass: %.2fkg", TotalMass);
    ImGui::Text("Hull mass: %.2fkg", HullMass);
    ImGui::Text("Wings mass: %.2fkg", WingsMass);
    ImGui::End();

    DebugDraw.SetupLegacyGL();

    b2World_Draw(World, &Drawer);

    if (b_ShowGravity) {
        const auto Grav = b2World_GetGravity(World);
        DebugDraw.DrawVector(TotalMass * Grav, COM, VectorScale, b2_colorForestGreen);
    }
    if (b_ShowTorques || b_ShowAerodynamicForces)
    {
        for (const auto ForcesData : Plane->GetLastAirfoilDynamicData()) {
            const auto drag = ForcesData.drag;
            const auto lift = ForcesData.lift;
            const auto loc = ForcesData.loc;
            const auto τ = ForcesData.torque;

            if (b_ShowAerodynamicForces) {
                DebugDraw.DrawVector(drag, loc, VectorScale, b2_colorRed);
                DebugDraw.Write("drag", loc + drag*VectorScale, b2_colorRed);

                DebugDraw.DrawVector(lift, loc, VectorScale, b2_colorAliceBlue);
                DebugDraw.Write("lift", loc + lift*VectorScale, b2_colorAliceBlue);
            }

            if (b_ShowTorques) {
                DebugDraw.DrawPseudoVector(τ, loc, PseudoVectorScale, .0f, b2_colorDarkCyan);
                DebugDraw.Write("Torque (aero)", loc + b2Vec2{static_cast<float>(τ)*PseudoVectorScale, .0f}, b2_colorDarkCyan);
            }
        }
    }

    if (b_ShowVerticalSpeed) {
        const auto [vx, vy] = Plane->GetVelocity();
        fix VertSpeed = b2Vec2{0, vy};
        DebugDraw.DrawVector(VertSpeed, COM, 1.0f, b2_colorWhite);
        DebugDraw.Write("Vert speed", COM + VertSpeed);
    }
}
