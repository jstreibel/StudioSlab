//
// Created by joao on 10/25/25.
//

#ifndef STUDIOSLAB_FLITTLEPLANE_H
#define STUDIOSLAB_FLITTLEPLANE_H

#include "box2d/box2d.h"
#include "Foils/Foil.h"

struct FWing {
    b2BodyId BodyId;
    TPointer<Foil::IAirfoilPolars> Airfoil;
    Foil::FAirfoilParams Params;
};

class FLittlePlane
{
public:
    explicit FLittlePlane(const Vector<FWing>& Wings) : Wings(Wings) {};
    static Foil::FAirfoilDynamicData ComputeForces(const FWing& Wing, const TPointer<LegacyGLDebugDraw>& DebugDraw=nullptr);

    Vector<FWing> Wings;

private:
    FLittlePlane() = default;
};

#endif //STUDIOSLAB_FLITTLEPLANE_H