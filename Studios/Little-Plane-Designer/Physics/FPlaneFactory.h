//
// Created by joao on 10/25/25.
//

#ifndef STUDIOSLAB_FPLANEFACTORY_H
#define STUDIOSLAB_FPLANEFACTORY_H

#include "FLittlePlane.h"

constexpr float ExpandedPolystyreneDensity = 11.0f;
constexpr float ExpandedPolystyreneFriction = 0.01f;
constexpr float ExpandedPolystyreneRestitution = 0.0f;

struct FWingDescriptor {
    float Density = ExpandedPolystyreneDensity;
    float Friction = ExpandedPolystyreneFriction;
    float Restitution = ExpandedPolystyreneRestitution;

    TPointer<Foil::IAirfoilPolars> Airfoil;
    Foil::FAirfoilParams Params;
    b2Vec2 RelativeLocation = b2Vec2{0.0f, 0.0f};
    float BaseAngle = 0.0f;
    float MaxAngle = 0.0f;
    float MinAngle = 0.0f;
    float OscFreq = 5.0f;
    float DampRatio = 0.7f;

    TPointer<FWing> Wing;
};

struct FBodyPartDescriptor {

    float Density = ExpandedPolystyreneDensity;
    float Friction = ExpandedPolystyreneFriction;
    float Restitution = ExpandedPolystyreneRestitution;

    float Width = 0.2f;
    float Height = 0.2f;

    float xOffset = 0.0f;
    float yOffset = 0.0f;

    float AngleRad = 0.0f;

};

class FPlaneFactory
{
    public:
    explicit FPlaneFactory() = default;

    FPlaneFactory& Reset();

    FPlaneFactory& AddWing(const FWingDescriptor &Descriptor);

    FPlaneFactory& AddBodyPart(const FBodyPartDescriptor &Descriptor);

    FPlaneFactory& SetPosition(b2Vec2);

    FPlaneFactory& SetRotation(float);

    TPointer<FLittlePlane> BuildPlane(b2WorldId World);
    b2BodyId BuildBody(b2WorldId World) const;

private:
    Vector<FWingDescriptor> WingDescriptors;
    Vector<FBodyPartDescriptor> BodyPartDescriptors;

    b2Vec2 Position = b2Vec2{0.0f, 0.0f};
    float  Angle    = 0.0f;

    TPointer<FWing> BuildWing(const FWingDescriptor &Descriptor, b2WorldId World) const;
    static void ShiftBodyCOM(float Δx, float Δy, b2BodyId Body);
};


#endif //STUDIOSLAB_FPLANEFACTORY_H