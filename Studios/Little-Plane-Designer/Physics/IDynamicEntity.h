//
// Created by joao on 11/4/25.
//

#ifndef STUDIOSLAB_IDYNAMICENTITY_H
#define STUDIOSLAB_IDYNAMICENTITY_H

#include "Utils/Pointer.h"
#include "../DebugDraw.h"

class IDynamicEntity {
public:
    virtual ~IDynamicEntity() = default;
    virtual void ComputeAndApplyForces(Slab::TPointer<LegacyGLDebugDraw>) = 0;
    virtual float GetTotalMass() const = 0;
    virtual b2Vec2 GetCenterOfMass_Global() const = 0;
};

#endif //STUDIOSLAB_IDYNAMICENTITY_H