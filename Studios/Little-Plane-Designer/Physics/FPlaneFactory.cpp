//
// Created by joao on 10/25/25.
//

#include "FPlaneFactory.h"

#include "graphic/graphic_basic.h"


FPlaneFactory& FPlaneFactory::Reset()
{
    return *this = FPlaneFactory();
}

FPlaneFactory& FPlaneFactory::AddWing(const FWingDescriptor &Descriptor)
{
    WingDescriptors.emplace_back(Descriptor);

    return *this;
}

FPlaneFactory& FPlaneFactory::AddBodyPart(const FBodyPartDescriptor& Descriptor) {
    BodyPartDescriptors.emplace_back(Descriptor);
    return *this;
}

FPlaneFactory& FPlaneFactory::SetPosition(const b2Vec2 Pos) {
    Position = Pos;
    return *this;
}

FPlaneFactory& FPlaneFactory::SetRotation(const float InitialAngle) {
    this->Angle = InitialAngle;
    return *this;
}

TPointer<FLittlePlane> FPlaneFactory::BuildPlane(const b2WorldId World) {

    const auto PlaneHull = BuildBody(World);

    Vector<TPointer<FWing>> Wings;
    for (auto &WingDesc : WingDescriptors) {
        WingDesc.Wing = BuildWing(WingDesc, World);
        Wings.emplace_back(WingDesc.Wing);

        const auto [xCom, yCom] = WingDesc.Params.COM;
        const auto [xAnchorLocal, yAnchorLocal] = WingDesc.Params.LocalAnchor;
        const auto Chord = WingDesc.Params.ChordLength;
        const auto Thick = WingDesc.Params.Thickness;
        auto Joint = b2DefaultRevoluteJointDef();
        Joint.bodyIdA = PlaneHull;
        Joint.bodyIdB = WingDesc.Wing->BodyId;
        Joint.collideConnected = false;
        Joint.localAnchorA = WingDesc.RelativeLocation;
        Joint.localAnchorB = {xCom * Chord + xAnchorLocal, yCom * Thick + yAnchorLocal};

        Joint.enableLimit = true;
        Joint.lowerAngle  = WingDesc.MinAngle;
        Joint.upperAngle  = WingDesc.MaxAngle;
        Joint.referenceAngle = WingDesc.BaseAngle;

        Joint.enableMotor = false;
        Joint.enableSpring = true;
        Joint.hertz = WingDesc.OscFreq;
        Joint.dampingRatio = WingDesc.DampRatio;

        fix RevJoint = b2CreateRevoluteJoint(World, &Joint);
        b2RevoluteJoint_SetTargetAngle(RevJoint, WingDesc.BaseAngle);
        b2RevoluteJoint_EnableSpring(RevJoint, true);

        WingDesc.Wing->RevJoint = RevJoint;
    }

    return New<FLittlePlane>(Wings);
}

b2BodyId FPlaneFactory::BuildBody(const b2WorldId World) const {
    b2BodyDef bodyDef = b2DefaultBodyDef();
    bodyDef.type = b2_dynamicBody;
    bodyDef.position = Position;
    bodyDef.rotation = b2MakeRot(Angle);
    bodyDef.linearDamping = 0.0f;
    const b2BodyId Body = b2CreateBody(World, &bodyDef);
    b2Body_SetName(Body, "Plane Body");

    b2ShapeDef ShapeDef = b2DefaultShapeDef();
    for (const auto & [Density, Friction, Restitution, Width, Height, xOffset, yOffset, AngleRad] : BodyPartDescriptors) {
        ShapeDef.density = Density;
        ShapeDef.material.friction = Friction;
        ShapeDef.material.restitution = Restitution;

        const auto Box = b2MakeOffsetBox(
            Width*.5f, Height*.5f,
            {xOffset, yOffset},
            b2MakeRot(AngleRad));
        b2CreatePolygonShape(Body, &ShapeDef, &Box);
    }

    return Body;
}

TPointer<FWing> FPlaneFactory::BuildWing(const FWingDescriptor& Descriptor, const b2WorldId World) const {
    const auto Params = Descriptor.Params;
    const auto Airfoil = Descriptor.Airfoil;

    // Dynamic box
    b2BodyDef bodyDef = b2DefaultBodyDef();

    bodyDef.type = b2_dynamicBody;
    bodyDef.position = {
        Position.x + Descriptor.RelativeLocation.x - Params.COM.x,
        Position.y + Descriptor.RelativeLocation.y - Params.COM.y
    };
    bodyDef.rotation = b2MakeRot(Descriptor.BaseAngle);

    const auto WingBody = b2CreateBody(World, &bodyDef);
    b2Body_SetName(WingBody, Params.Name.c_str());

    // Rectangle with c/4 at (0,0):
    // LE=-0.25c, TE=+0.75c

    fix Chord = Params.ChordLength;
    fix Thick = Params.Thickness;

    // 1) Build hull
    b2Hull hull;
    {
        const b2Vec2 pts[4] = {
            { -0.5f*Chord, -0.5f*Thick },
            {  0.5f*Chord, -0.5f*Thick },
            {  0.5f*Chord,  0.5f*Thick },
            { -0.5f*Chord,  0.5f*Thick }
        }; // in COM reference frame
        hull = b2ComputeHull(pts, 4);
    }

    // 2) Make convex polygon from hull
    const b2Polygon WingShape = b2MakePolygon(&hull, 0.0f);

    // 3) Create a fixture
    b2ShapeDef sdef = b2DefaultShapeDef();
    sdef.density = 1.0f;
    sdef.material.friction = 0.1f;
    b2CreatePolygonShape(WingBody, &sdef, &WingShape);

    auto [COM_x, COM_y] = Params.COM;
    ShiftBodyCOM(COM_x*Chord, COM_y*Thick, WingBody);

    return New<FWing>(FWing{
        .BodyId = WingBody,
        .Airfoil = Airfoil,
        .Params = Params,
        .BaseAngle = Descriptor.BaseAngle,
        .MaxAngle = Descriptor.MaxAngle,
        .MinAngle = Descriptor.MinAngle});
}

void FPlaneFactory::ShiftBodyCOM(const float Δx, const float Δy, const b2BodyId Body) {
    // target COM shift (local body frame)

    // md0.mass, md0.center, md0.I (I about body origin)
    auto [mass, center, rotationalInertia] = b2Body_GetMassData(Body);

    const float m  = mass;
    const auto [x, y] = center;

    // inertia about current COM
    const float I_com = rotationalInertia - m * (x*x + y*y);

    // new local center
    const auto c1 = (b2Vec2){ Δx, Δy };

    // inertia about body origin with new COM
    const float I1 = I_com + m * (c1.x*c1.x + c1.y*c1.y);

    // apply
    const b2MassData md1 = { .mass = m, .center = c1, .rotationalInertia = I1 };
    b2Body_SetMassData(Body, md1);
    b2Body_SetAwake(Body, true);
}