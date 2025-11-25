//
// Created by joao on 10/25/25.
//

#ifndef STUDIOSLAB_FLITTLEPLANE_H
#define STUDIOSLAB_FLITTLEPLANE_H

#include "../Physics/FAtmosphericCondition.h"
#include "../Interfaces.h"
#include "box2d/box2d.h"
#include "../Physics/Foils/Foil.h"
#include "../../../Lib/Graphics/Interfaces/IDrawable.h"
#include "Graphics/OpenGL/Texture2D_Color.h"

struct FWing {
    b2BodyId BodyId;
    b2JointId RevJoint;
    TPointer<Foil::IAirfoilPolars> Airfoil;
    Foil::FAirfoilParams Params;
    float BaseAngle = 0.0f;
    const float MaxAngle  = 0.0f;
    const float MinAngle  = 0.0f;

    void SetBaseAngle(const double Delta) {
        fix CurrentAngle = b2RevoluteJoint_GetTargetAngle(RevJoint);
        fix NewAngle = CurrentAngle + Delta;

        if (NewAngle < MinAngle || NewAngle > MaxAngle) return;
        BaseAngle = NewAngle;
        b2RevoluteJoint_SetTargetAngle(RevJoint, NewAngle);
        b2Joint_WakeBodies(RevJoint);
    }
};

class FLittlePlane final : public Graphics::IDrawable2D, public IDynamicEntity
{
public:
    static Foil::FAirfoilDynamicData ComputeForces(
        const FWing& Wing,
        const FAtmosphericCondition &Atmosphere={});

    const Vector<Foil::FAirfoilDynamicData>& GetLastAirfoilDynamicData() const { return LastAirfoilDynamicData; }

    void ComputeAndApplyForces() override;
    float GetTotalMass() const override;
    float ComputeHullMass() const;
    float ComputeWingsMass() const;
    b2Vec2 GetCenterOfMass_Global() const override;
    b2Vec2 GetPosition() const override;
    b2Vec2 GetVelocity() const override;

    void Draw(const Graphics::FDraw2DParams&) override;
    const FWing& GetWing(int i) const { return *Wings[i]; }
    int GetWingCount() const { return static_cast<int>(Wings.size()); }
    void AdjustWingAngle(int WingId, double Delta) const;

private:
    friend class FPlaneFactory;

    Vector<Foil::FAirfoilDynamicData> LastAirfoilDynamicData;

    explicit FLittlePlane(const Vector<TPointer<FWing>>& Wings, const b2BodyId HullBody);
    FLittlePlane() = default;

    Vector<TPointer<FWing>> Wings;
    b2BodyId HullBody;

    TPointer<Graphics::OpenGL::FTexture2D_Color> HullTexture;
};

#endif //STUDIOSLAB_FLITTLEPLANE_H
