//
// Created by joao on 10/25/25.
//

#ifndef STUDIOSLAB_FPLANEFACTORY_H
#define STUDIOSLAB_FPLANEFACTORY_H

#include "FLittlePlane.h"
#include "../Physics/Materials.h"
#include "Math/Geometry/Geometry.h"

struct IBodyPartBlueprintRenderer {
    virtual ~IBodyPartBlueprintRenderer() = default;

    virtual auto GetLeftView() const -> Math::FPointSet = 0;
    virtual auto GetTopView() const -> Math::FPointSet = 0;
};

struct FWingDescriptor {
    float Density = ExpandedPolystyrene.Density;
    float Friction = ExpandedPolystyrene.Friction;
    float Restitution = ExpandedPolystyrene.Restitution;

    TPointer<Foil::IAirfoilPolars> Airfoil;
    Foil::FAirfoilParams Params;
    Math::Real2D RelativeLocation = Math::Real2D{0.0f, 0.0f};
    float BaseAngle = 0.0f;
    float MaxAngle = 0.0f;
    float MinAngle = 0.0f;
    float OscFreq = 5.0f;
    float DampRatio = 0.7f;

    TPointer<FWing> Wing;
};

struct FWingDescriptorRenderer final : IBodyPartBlueprintRenderer {
    auto GetLeftView() const -> Math::FPointSet override;
    auto GetTopView() const -> Math::FPointSet override;

    FWingDescriptorRenderer() = delete;
    explicit FWingDescriptorRenderer(const FWingDescriptor &Descriptor) : Descriptor(Descriptor) {}

    FWingDescriptor Descriptor;
};

struct FBodyPartDescriptor final {
    float Density = ExpandedPolystyreneDensity;
    float Friction = ExpandedPolystyreneFriction;
    float Restitution = ExpandedPolystyreneRestitution;

    float Length = 0.2f;
    float Height = 0.2f;
    // Out-of-plane thickness/depth [m] to convert 3D density (kg/m^3)
    // to Box2D's 2D density (kg/m^2): density_2d = Density * Depth
    float Depth = 0.30f;

    float xOffset = 0.0f;
    float yOffset = 0.0f;

    float AngleRad = 0.0f;
};

struct FBodyPartRenderer final : IBodyPartBlueprintRenderer {
    FBodyPartDescriptor Descriptor;

    auto GetLeftView() const -> Math::FPointSet override;
    auto GetTopView() const -> Math::FPointSet override;

    FBodyPartRenderer() = delete;
    explicit FBodyPartRenderer(const FBodyPartDescriptor& Descriptor) : Descriptor(Descriptor) {}
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

    Vector<FWingDescriptor> WingDescriptors;
    Vector<FBodyPartDescriptor> BodyPartDescriptors;

    b2Vec2 Position = b2Vec2{0.0f, 0.0f};
    float  Angle    = 0.0f;

    TPointer<FWing> BuildWing(const FWingDescriptor &Descriptor, b2WorldId World) const;
    static void ShiftBodyCOM(float Δx, float Δy, b2BodyId Body);

};


#endif //STUDIOSLAB_FPLANEFACTORY_H
