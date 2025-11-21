//
// Created by joao on 11/20/25.
//

#include "FLPDPhysicsEngine.h"

constexpr int SubSteps = 1;
constexpr int ManualSubSteps = 10;

FLittlePlaneDesignerPhysicsEngine::FLittlePlaneDesignerPhysicsEngine() {
    auto worldDef = b2DefaultWorldDef();
    worldDef.gravity = (b2Vec2){0.0f, -9.8f};
    World = b2CreateWorld(&worldDef);
}

void FLittlePlaneDesignerPhysicsEngine::Tick(const Miliseconds ElapsedTime) {
    if (!b_IsRunning) return;

    const auto TimeStepSeconds = ElapsedTime/1000.;

    const auto SplitTimeStep = TimeStepSeconds/ManualSubSteps;
    for (int i = 0; i < ManualSubSteps; ++i) {
        if (Plane) Plane->ComputeAndApplyForces();
        b2World_Step(World, SplitTimeStep, SubSteps);
    }
}

void FLittlePlaneDesignerPhysicsEngine::TogglePause() {
    b_IsRunning = !b_IsRunning;
}

