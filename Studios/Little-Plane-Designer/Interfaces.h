//
// Created by joao on 11/6/25.
//

#ifndef STUDIOSLAB_INTERFACES_H
#define STUDIOSLAB_INTERFACES_H

#include "Graphics/Backend/Events/KeyboardState.h"
#include "Math/Point.h"
#include "Render/DebugDraw.h"

using Seconds = float;

struct ILocatableEntity {
    virtual ~ILocatableEntity() = default;
    virtual Slab::Math::Point2D GetPosition() const = 0;
};

struct IMovingEntity : ILocatableEntity {
    virtual Slab::Math::Point2D GetVelocity() const = 0;
};

struct IDynamicEntity : IMovingEntity {
    virtual void ComputeAndApplyForces() = 0;
    virtual float GetTotalMass() const = 0;
    virtual Slab::Math::Point2D GetCenterOfMass_Global() const = 0;

    virtual float GetAngle() const = 0;
};

struct FInputState {
    const Slab::Graphics::FKeyboardState& KeyboardState;
    const Slab::Graphics::FMouseState& MouseState;
};

struct IInputStateReader {
    virtual ~IInputStateReader() = default;

    virtual void HandleInputState(FInputState) {};

    virtual bool NotifyKeyboard(Slab::Graphics::EKeyMap, Slab::Graphics::EKeyState, Slab::Graphics::EModKeys)
    { return false; }
};

struct IUpdateable {
    virtual ~IUpdateable() = default;
    virtual void Tick(Seconds ElapsedTime) = 0;
    virtual void TogglePause() = 0;
};

#endif //STUDIOSLAB_INTERFACES_H