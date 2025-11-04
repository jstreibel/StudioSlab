//
// Created by joao on 10/25/25.
//

#ifndef STUDIOSLAB_FLITTLEPLANE_H
#define STUDIOSLAB_FLITTLEPLANE_H

#include "FAtmosphericCondition.h"
#include "box2d/box2d.h"
#include "Foils/Foil.h"

struct FWing {
    b2BodyId BodyId;
    b2JointId RevJoint;
    TPointer<Foil::IAirfoilPolars> Airfoil;
    Foil::FAirfoilParams Params;
    const float BaseAngle = 0.0f;
    const float MaxAngle  = 0.0f;
    const float MinAngle  = 0.0f;

};

class FLittlePlane
{
public:
    explicit FLittlePlane(const Vector<TPointer<FWing>>& Wings) : Wings(Wings) {};
    static Foil::FAirfoilDynamicData ComputeForces(
        const FWing& Wing,
        const FAtmosphericCondition &Atmosphere={},
        const TPointer<LegacyGLDebugDraw>& DebugDraw=nullptr);

    Vector<TPointer<FWing>> Wings;

private:
    FLittlePlane() = default;
};

#endif //STUDIOSLAB_FLITTLEPLANE_H