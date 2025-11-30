//
// Created by joao on 10/25/25.
//

#include "FPlaneFactory.h"

#include "Core/Tools/Log.h"
#include "graphic/graphic_basic.h"
#include "../Defaults.h"
#include "../Utils.h"
#include "Math/Geometry/Geometry.h"

auto FWingDescriptorUtils::GetLeftView() const -> Math::Geometry::FPolygon {
    fix &Airfoil = Descriptor.Airfoil;
    fix &Params = Descriptor.Params;
    fix &RelativeLocation = Descriptor.RelativeLocation;

    auto Vertices = Airfoil->GetProfileVertices(DefaultAirfoilProfileNumSegments, Params.ChordLength, Params.ThicknessInUnitsOfChortLength);

    const auto Anchor = Params.LocalAnchor;

    return Math::Geometry::FPolygon{Vertices.Translate(RelativeLocation + Anchor)};
}

auto FWingDescriptorUtils::GetTopView() const -> Math::Geometry::FPolygon {
    fix &Params = Descriptor.Params;
    fix &RelativeLocation = Descriptor.RelativeLocation;

    fix ChordLength = Params.ChordLength;
    fix WingSpan = Params.Span;
    fix AnchorX = Params.LocalAnchor.x + ChordLength/2;
    fix RelativeLocationX = RelativeLocation.x;

    Math::FPointSet Points{};

    fix xMin = -ChordLength*.5f;
    fix yMin = -WingSpan*.5f;
    fix xMax = ChordLength*.5f;
    fix yMax = WingSpan*.5f;

    return Math::Geometry::FPolygon{Math::Geometry::FAABox{{xMin, yMin}, {xMax, yMax}}.GetPoints().Translate({RelativeLocationX+AnchorX, 0.0})};
}

auto FWingDescriptorUtils::GetFrontView() const -> Math::Geometry::FPolygon {
    NOT_IMPLEMENTED_CLASS_METHOD

}

auto FWingDescriptorUtils::ComputeMassProperties() const -> Math::Geometry::FMass2DProperties {
    fix &Airfoil = Descriptor.Airfoil;
    fix &Params = Descriptor.Params;
    fix Density = Descriptor.Density;

    const auto Vertices = Airfoil->GetProfileVertices(DefaultAirfoilProfileNumSegments, Params.ChordLength, Params.ThicknessInUnitsOfChortLength);

    return Vertices.ComputeMassProperties(Density*Params.Span);
}

auto FBodyPartRenderer::GetLeftView() const -> Math::Geometry::FPolygon {
    return Descriptor.Section;
}

auto FBodyPartRenderer::GetTopView() const -> Math::Geometry::FPolygon {
    Math::FPoint2DVec Points;

    fix xMin = Descriptor.Section.GetMin().x;
    fix yMin = -Descriptor.Depth*.5f;
    fix xMax = Descriptor.Section.GetMax().x;
    fix yMax = Descriptor.Depth*.5f;

    Points.emplace_back(xMin, yMin);
    Points.emplace_back(xMax, yMin);
    Points.emplace_back(xMax, yMax);
    Points.emplace_back(xMin, yMax);

    return Math::Geometry::FPolygon(Points);
}

auto FBodyPartRenderer::GetFrontView() const -> Math::Geometry::FPolygon {
    NOT_IMPLEMENTED_CLASS_METHOD
}

FPlaneFactory& FPlaneFactory::Reset() { return *this = FPlaneFactory(); }

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

    const auto HullBody = BuildBody(World);

    Vector<TPointer<FWing>> Wings;
    for (auto &WingDesc : WingDescriptors) {
        WingDesc.Wing = BuildWing(WingDesc, World);
        Wings.emplace_back(WingDesc.Wing);

        const auto [xWingAnchorLocal, yWingAnchorLocal] = WingDesc.Params.LocalAnchor;
        // const auto Chord = WingDesc.Params.ChordLength;
        // const auto Thick = WingDesc.Params.Thickness;
        auto Joint = b2DefaultRevoluteJointDef();
        Joint.bodyIdA = HullBody;
        Joint.bodyIdB = WingDesc.Wing->BodyId;
        Joint.collideConnected = false;
        auto &HullAnchor = Joint.localAnchorA;
        auto &WingAnchor = Joint.localAnchorB;
        HullAnchor = {
            .x = static_cast<float>(WingDesc.RelativeLocation.x),
            .y = static_cast<float>(WingDesc.RelativeLocation.y)
        };
        WingAnchor = {
            .x = static_cast<float>(xWingAnchorLocal),
            .y = static_cast<float>(yWingAnchorLocal)
        };

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

    return TPointer<FLittlePlane>(new FLittlePlane(Wings, HullBody));
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
    for (const auto & Desc : BodyPartDescriptors) {
        // Convert 3D density (kg/m^3) -> 2D density (kg/m^2) via effective depth
        ShapeDef.density = Desc.Density * Desc.Depth;
        ShapeDef.material.friction = Desc.Friction;
        ShapeDef.material.restitution = Desc.Restitution;

        const auto &Pts = Desc.Section.GetPoints();
        if (Pts.size() < 3) {
            Core::Log::Error("Body part polygon has fewer than 3 points; skipping");
            continue;
        }

        auto poly = b2Polygon{};
        bool b_PolyOk = false;
        if (Pts.size() <= B2_MAX_POLYGON_VERTICES) {
            // Convert to b2Vec2 array
            Vector<b2Vec2> verts;
            verts.reserve(Pts.size());
            for (const auto &p : Pts) {
                verts.push_back({static_cast<float>(p.x), static_cast<float>(p.y)});
            }

            // Compute convex hull and create a Box2D polygon
            b2Hull hull = b2ComputeHull(verts.data(), static_cast<int>(verts.size()));
            if (hull.count < 3) {
                Core::Log::Error("Box2D hull generation failed for body part; using bounding box");
            }

            poly = b2MakePolygon(&hull, 0.0f);
            b_PolyOk = true;
        }
        if (!b_PolyOk) {
            fix BoundingBox = Desc.Section.GetBoundingBox();
            fix Pos = BoundingBox.Center();
            poly = b2MakeOffsetBox(BoundingBox.Width()/2, BoundingBox.Height()/2, b2Vec2_FromPoint2D(Pos), b2MakeRot(0.0f));
        }

        b2CreatePolygonShape(Body, &ShapeDef, &poly);
    }

    return Body;
}

TPointer<FWing> FPlaneFactory::BuildWing(const FWingDescriptor& Descriptor, const b2WorldId World) const {
    const auto Params = Descriptor.Params;
    const auto Airfoil = Descriptor.Airfoil;

    b2BodyDef bodyDef = b2DefaultBodyDef();

    bodyDef.type = b2_dynamicBody;
    bodyDef.position = {
        static_cast<float>(Position.x + Descriptor.RelativeLocation.x),
        static_cast<float>(Position.y + Descriptor.RelativeLocation.y)
    };
    bodyDef.rotation = b2MakeRot(Descriptor.BaseAngle);

    const auto WingBody = b2CreateBody(World, &bodyDef);
    b2Body_SetName(WingBody, Params.Name.c_str());

    // Rectangle with c/4 at (0,0):
    // LE=-0.25c, TE=+0.75c

    fix Chord = Params.ChordLength;
    fix Thick = Params.ThicknessInUnitsOfChortLength * Params.ChordLength;

    const b2Polygon WingShape = b2MakeBox(Chord*.5f, Thick*.5);

    b2ShapeDef sdef = b2DefaultShapeDef();
    // Convert 3D density (kg/m^3) -> 2D density (kg/m^2) using span as out-of-plane depth
    sdef.density = Descriptor.Density * Params.Span;
    sdef.material.friction = Descriptor.Friction;
    sdef.material.restitution = Descriptor.Restitution;
    b2CreatePolygonShape(WingBody, &sdef, &WingShape);

    // auto [COM_x, COM_y] = Params.COM;
    // ShiftBodyCOM(COM_x*Chord, COM_y*Thick, WingBody);

    {
        fix WingMassProperties = FWingDescriptorUtils{Descriptor}.ComputeMassProperties();
        b2MassData MassData = {
            .mass = static_cast<float>(WingMassProperties.Mass),
            .center = {static_cast<float>(WingMassProperties.Centroid.x), static_cast<float>(WingMassProperties.Centroid.y)},
            .rotationalInertia = static_cast<float>(WingMassProperties.InertiaOrigin)
        };

        b2Body_SetMassData(WingBody, MassData);

        auto Mass = b2Body_GetMass(WingBody);
        auto I = b2Body_GetRotationalInertia(WingBody);
    }

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
