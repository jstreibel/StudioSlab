//
// Created by joao on 11/20/25.
//

#include "FPlaneController.h"

#include "Utils/Angles.h"

FPlaneController::FPlaneController(TPointer<FLittlePlane> Plane) : Plane(Plane) {}

void FPlaneController::HandleInputState(FInputState InputState) {

    auto &KeyboardState = InputState.KeyboardState;

    /*
    if (KeyboardState.IsPressed(Graphics::Key_MINUS)) {
        Camera.Zoom(1.01f);
    } if (KeyboardState.IsPressed(Graphics::Key_EQUAL)) {
        Camera.Zoom(.99f);
    }

    if (KeyboardState.IsPressed(Graphics::Key_LEFT_SHIFT)) {
        fix PanDelta = Camera.GetWidth()*.005f;
        if (KeyboardState.IsPressed(Graphics::Key_LEFT))    Camera.Pan({-PanDelta, 0.0f});
        if (KeyboardState.IsPressed(Graphics::Key_RIGHT))   Camera.Pan({ PanDelta, 0.0f});
        if (KeyboardState.IsPressed(Graphics::Key_UP))      Camera.Pan({0.0f,  PanDelta});
        if (KeyboardState.IsPressed(Graphics::Key_DOWN))    Camera.Pan({0.0f, -PanDelta});
    }
    */
    {
        bool Flaps = false;
        const auto Wing = Plane->GetWing(0);
        const auto Δα = DegToRad(0.25f);
        if (KeyboardState.IsPressed(Graphics::Key_UP)) {
            Plane->AdjustWingAngle(0, Δα);
        }
        if (KeyboardState.IsPressed(Graphics::Key_DOWN)) {
            Plane->AdjustWingAngle(0, -Δα);
        }
        if (KeyboardState.IsPressed(Graphics::Key_LEFT)) {
            fix RevJoint = Wing.RevJoint;
            fix WingBody = Wing.BodyId;

            b2RevoluteJoint_SetTargetAngle(RevJoint, Wing.MinAngle);
            b2Body_SetAwake(WingBody, true);
            Flaps = true;
        }
        if (KeyboardState.IsPressed(Graphics::Key_RIGHT)) {
            fix RevJoint = Wing.RevJoint;
            fix WingBody = Wing.BodyId;

            b2RevoluteJoint_SetTargetAngle(RevJoint, Wing.MaxAngle);
            b2Body_SetAwake(WingBody, true);
            Flaps = !Flaps; // little trick, figure why
        }
        if (!Flaps) {
            fix RevJoint = Wing.RevJoint;
            b2RevoluteJoint_SetTargetAngle(RevJoint, Wing.BaseAngle);
        }

    }
}

bool FPlaneController::NotifyKeyboard(Graphics::EKeyMap key, Graphics::EKeyState state, Graphics::EModKeys modKeys) {
    return false;
}