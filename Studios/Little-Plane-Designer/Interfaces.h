//
// Created by joao on 11/6/25.
//

#ifndef STUDIOSLAB_INTERFACES_H
#define STUDIOSLAB_INTERFACES_H

#include "Utils/Pointer.h"
#include "DebugDraw.h"

class ILocatableEntity {
public:
    virtual ~ILocatableEntity() = default;
    virtual b2Vec2 GetPosition() const = 0;
};

class IMovingEntity : public ILocatableEntity {
public:
    virtual b2Vec2 GetVelocity() const = 0;
};

class IDynamicEntity : public IMovingEntity {
public:
    virtual void ComputeAndApplyForces(Slab::TPointer<LegacyGLDebugDraw>) = 0;
    virtual float GetTotalMass() const = 0;
    virtual b2Vec2 GetCenterOfMass_Global() const = 0;
};

class IUpdateable {
public:
    virtual ~IUpdateable() = default;
    virtual void Update(float ElapsedTimeMsec) = 0;

};

#endif //STUDIOSLAB_INTERFACES_H