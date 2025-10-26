//
// Created by joao on 10/25/25.
//

#ifndef STUDIOSLAB_FPLANEFACTORY_H
#define STUDIOSLAB_FPLANEFACTORY_H

#include "FLittlePlane.h"

struct FWingDescriptor {
    TPointer<Foil::IAirfoilPolars> Airfoil;
    Foil::FAirfoilParams Params;
    b2Vec2 RelativeLocation = b2Vec2{0.0f, 0.0f};
    float Angle = 0.0f;
};

class FPlaneFactory
{
    public:
    explicit FPlaneFactory() = default;


    FPlaneFactory& AddWing(const FWingDescriptor &Descriptor);

    FPlaneFactory& SetPosition(b2Vec2);

    TPointer<FLittlePlane> BuildPlane(b2WorldId World);

private:
    Vector<FWingDescriptor> Descriptors;

    b2Vec2 Position = b2Vec2{0.0f, 0.0f};
    float  Angle    = 0.0f;
    Vector<FWing> Wings;

    static FWing BuildWing(const FWingDescriptor &Descriptor, b2WorldId World);
    static void ShiftBodyCOM(float Δx, float Δy, b2BodyId Body);
};


#endif //STUDIOSLAB_FPLANEFACTORY_H