//
// Created by joao on 11/6/25.
//

#ifndef STUDIOSLAB_INTERFACES_H
#define STUDIOSLAB_INTERFACES_H

#include "Graphics/Backend/Events/KeyboardState.h"
#include "Render/DebugDraw.h"

using Seconds = float;

struct ILocatableEntity {
    virtual ~ILocatableEntity() = default;
    virtual b2Vec2 GetPosition() const = 0;
};

struct IMovingEntity : ILocatableEntity {
    virtual b2Vec2 GetVelocity() const = 0;
};

struct IDynamicEntity : IMovingEntity {
    virtual void ComputeAndApplyForces() = 0;
    virtual float GetTotalMass() const = 0;
    virtual b2Vec2 GetCenterOfMass_Global() const = 0;
};

struct FInputState {
    const Slab::Graphics::FKeyboardState& KeyboardState;
    const Slab::Graphics::FMouseState& MouseState;
};

struct IInputStateReader {
    virtual ~IInputStateReader() = default;

    virtual void HandleInputState(FInputState) = 0;
};

struct IUpdateable {
    virtual ~IUpdateable() = default;
    virtual void Tick(Seconds ElapsedTime) = 0;
    virtual void TogglePause() = 0;
};

#endif //STUDIOSLAB_INTERFACES_H