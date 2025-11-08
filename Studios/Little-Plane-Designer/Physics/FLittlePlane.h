//
// Created by joao on 10/25/25.
//

#ifndef STUDIOSLAB_FLITTLEPLANE_H
#define STUDIOSLAB_FLITTLEPLANE_H

#include "FAtmosphericCondition.h"
#include "../Interfaces.h"
#include "box2d/box2d.h"
#include "Foils/Foil.h"
#include "Graphics/IDrawable.h"

struct FWing {
    b2BodyId BodyId;
    b2JointId RevJoint;
    TPointer<Foil::IAirfoilPolars> Airfoil;
    Foil::FAirfoilParams Params;
    const float BaseAngle = 0.0f;
    const float MaxAngle  = 0.0f;
    const float MinAngle  = 0.0f;

};

class FLittlePlane final : public Graphics::IDrawable, public IDynamicEntity
{
public:
    static Foil::FAirfoilDynamicData ComputeForces(
        const FWing& Wing,
        const FAtmosphericCondition &Atmosphere={},
        const TPointer<LegacyGLDebugDraw>& DebugDraw=nullptr);

    void ComputeAndApplyForces(TPointer<LegacyGLDebugDraw>) override;
    float GetTotalMass() const override;
    b2Vec2 GetCenterOfMass_Global() const override;
    b2Vec2 GetPosition() const override;
    b2Vec2 GetVelocity() const override;

    void Draw(const Graphics::FPlatformWindow&) override;
    const FWing& GetWing(int i) const { return *Wings[i]; }

private:
    friend class FPlaneFactory;

    explicit FLittlePlane(const Vector<TPointer<FWing>>& Wings, const b2BodyId HullBody)
    : Wings(Wings), HullBody(HullBody) {}
    FLittlePlane() = default;

    Vector<TPointer<FWing>> Wings;
    b2BodyId HullBody;
};

#endif //STUDIOSLAB_FLITTLEPLANE_H