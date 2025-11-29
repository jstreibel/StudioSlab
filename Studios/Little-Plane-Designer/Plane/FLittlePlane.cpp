//
// Created by joao on 10/25/25.
//

#include "FLittlePlane.h"

#include "../Physics/FAtmosphericCondition.h"
#include "Core/Tools/Log.h"
#include "../Physics/Foils/Foil.h"
#include "Graphics/OpenGL/Texture2D.h"
#include "Graphics/OpenGL/Texture2D_Color.h"
#include "Graphics/OpenGL/LegacyGL/PointSetRenderer.h"
#include "Graphics/Plot2D/PlotStyle.h"
#include "Graphics/SFML/Graph.h"
#include "../Defaults.h"
#include "../Utils.h"

Foil::FAirfoilDynamicData FLittlePlane::ComputeForces(
    const FWing& Wing,
    const FAtmosphericCondition &Atmosphere)
{
    // Directions
    const b2Vec2 FW_Unit = b2Body_GetWorldVector(Wing.BodyId, b2Vec2(-1.0f, 0.0f));

    // Kinematics at c/4: use WORLD CENTER, not position
    const b2Vec2 COM = b2Body_GetWorldCenterOfMass(Wing.BodyId);
    const b2Vec2 VelC4 = b2Body_GetLinearVelocity(Wing.BodyId);
    // const float VelC4_Mag = b2Length(VelC4);
    const b2Vec2 VelC4_Unit = b2Normalize(VelC4);
    const float VelC4_Mag = b2Length(VelC4);
    const float AoA_cos = b2Dot(FW_Unit, VelC4_Unit);
    const float AoA_sin = b2Cross(FW_Unit, VelC4_Unit);
    const float AoA = std::atan2(AoA_sin, AoA_cos);
    const float ω = b2Body_GetAngularVelocity(Wing.BodyId);

    // Wind
    const b2Vec2 WindOnPoint = -VelC4;
    const float  WindOnPointLen    = b2Length(WindOnPoint);
    if (WindOnPointLen < 1e-4f) return Foil::FAirfoilDynamicData::Null();
    const b2Vec2 WindOnPointUnit = b2Normalize(WindOnPoint); // TODO: could take advantage of already computed length above

    // Coefficients
    const double Cl = Wing.Airfoil->Cl(AoA);
    const double Cd = Wing.Airfoil->Cd(AoA);
    const double Cm_c4 = Wing.Airfoil->Cm_c4(AoA);

    // Params:
    const auto rho = Atmosphere.rho;
    const auto chord_length = Wing.Params.ChordLength;
    const auto span = Wing.Params.Span;

    constexpr struct RotDampParams {
        float Cmq    = -0.1f;   // nondimensional pitch-rate derivative
        float Komega = 0.1f;     // extra viscous damping [N·m·s] per unit span
    } P;

    // Dynamic pressure (keep density sane)
    const double DynamicPressure   = 0.5 * rho * static_cast<double>(WindOnPointLen) * static_cast<double>(WindOnPointLen);
    const double SurfaceArea   = chord_length * span;

    // Magnitudes
    const double Lmag = DynamicPressure * SurfaceArea * Cl;
    const double Dmag = DynamicPressure * SurfaceArea * Cd;
    const double τ_mag = 0.5 * DynamicPressure * span * chord_length * chord_length * Cm_c4;
    const double τ_aero = 0.25 * rho * VelC4_Mag * chord_length * chord_length* chord_length * P.Cmq * ω;
    const double τ_visc = -P.Komega * ω;
    const double τ = τ_mag + τ_aero + τ_visc;

    // Forces
    const b2Vec2 drag = -static_cast<float>(-Dmag) * WindOnPointUnit;
    const b2Vec2 lift = +static_cast<float>(+Lmag) * b2Vec2(-WindOnPointUnit.y, WindOnPointUnit.x);

    return Foil::FAirfoilDynamicData{
        .drag = drag,
        .lift = lift,
        .loc = COM,
        .torque = static_cast<float>(τ),
        .Cl = static_cast<float>(Cl),
        .Cd = static_cast<float>(Cd),
        .Cm_c4 = static_cast<float>(Cm_c4),
        .AoA = AoA
        };
}

void FLittlePlane::ComputeAndApplyForces() {
    LastAirfoilDynamicData.clear();

    for (auto &Wing : Wings) {
        const auto &WingBody = Wing->BodyId;
        if (!b2Body_IsAwake(WingBody)) continue;

        auto ForcesData = ComputeForces(*Wing, {});
        LastAirfoilDynamicData.emplace_back(ForcesData);

        b2Body_ApplyForce (WingBody, ForcesData.GetTotalForce(), ForcesData.loc, true);
        b2Body_ApplyTorque(WingBody, ForcesData.torque, true);
    }
}

float FLittlePlane::GetTotalMass() const {
    return ComputeHullMass() + ComputeWingsMass();
}

float FLittlePlane::ComputeHullMass() const {
    return b2Body_GetMass(HullBody);
}

float FLittlePlane::ComputeWingsMass() const {
    float Mass = 0.0f;

    static bool FirstTime = true;

    for (const auto &Wing : Wings) {
        fix WingMass = b2Body_GetMass(Wing->BodyId);
        Mass += WingMass;

        if (FirstTime) Core::Log::Info(ToStr("Wing mass: %f", WingMass));
    }

    FirstTime = false;

    return Mass;
}

Math::Point2D FLittlePlane::GetCenterOfMass_Global() const {
    fix HullCOM = b2Body_GetWorldCenterOfMass(HullBody);
    fix HullMass = b2Body_GetMass(HullBody);

    Math::Point2D COM_Accumulator = Point2D_Fromb2Vec2(HullMass * HullCOM);
    auto TotalMass = HullMass;

    for (const auto &Wing : Wings) {
        fix WingCOM = b2Body_GetWorldCenterOfMass(Wing->BodyId);
        fix WingMass = b2Body_GetMass(Wing->BodyId);

        COM_Accumulator += Point2D_Fromb2Vec2(WingMass * WingCOM);
        TotalMass += WingMass;
    }

    return COM_Accumulator * (1.f/TotalMass);
}

Math::Point2D FLittlePlane::GetPosition() const { return Point2D_Fromb2Vec2(b2Body_GetPosition(HullBody)); }

Math::Point2D FLittlePlane::GetVelocity() const {
    return Point2D_Fromb2Vec2(b2Body_GetLinearVelocity(HullBody));
}

float FLittlePlane::GetAngle() const {
    return b2Rot_GetAngle(b2Body_GetRotation(HullBody));
}

void FLittlePlane::Draw(const Graphics::FDraw2DParams& DrawParams) {

    // Draw a 2x2 textured rectangle centered at the hull body using HullTexture
    const auto [xBody, yBody] = b2Body_GetPosition(HullBody);
    const auto [c, s] = b2Body_GetRotation(HullBody);

    if (HullTexture != nullptr)
    {
        // World-space half-size (2x2 square)
        constexpr float HalfWidth = 2.298f;
        constexpr float HalfHeight = HalfWidth;
        constexpr float dx = -0.05f;
        constexpr float dy = 0.0f;

        auto xform = [&](const float lx, const float ly) -> std::pair<double,double> {
            // Apply body rotation and translation
            const auto x = lx + dx;
            const auto y = ly + dy;
            const double X = xBody + x*c - y*s;
            const double Y = yBody + x*s + y*c;
            return {X, Y};
        };

        // Local corners (counter-clockwise): bottom-left, bottom-right, top-right, top-left
        const auto [blx, bly] = xform(-HalfWidth, -HalfHeight);
        const auto [brx, bry] = xform( +HalfWidth, -HalfHeight);
        const auto [trx, try_] = xform( +HalfWidth,  +HalfHeight);
        const auto [tlx, tly] = xform(-HalfWidth,  +HalfHeight);

        // Bind texture and render immediate-mode quad
        Graphics::OpenGL::Legacy::SetupLegacyGL();
        Graphics::OpenGL::FTexture::EnableTextures();
        HullTexture->Activate();
        HullTexture->Bind();


        // White modulation to preserve original texture colors
        glColor4f(1.f, 1.f, 1.f, 1.f);

        glBegin(GL_QUADS);
        {
            // Use flipped V to compensate for typical top-left image origin
            constexpr auto z_coord = -0.1;
            glTexCoord2f(0.f, 1.f); glVertex3d(blx, bly, z_coord);
            glTexCoord2f(1.f, 1.f); glVertex3d(brx, bry, z_coord);
            glTexCoord2f(1.f, 0.f); glVertex3d(trx, try_, z_coord);
            glTexCoord2f(0.f, 0.f); glVertex3d(tlx, tly, z_coord);
        }
        glEnd();

        // Cleanup state
        Graphics::OpenGL::FTexture::Deactivate();
        Graphics::OpenGL::FTexture::DisableTextures();
    }

    namespace Drawer = Graphics::OpenGL::Legacy;

    Graphics::PlotStyle WingStyle{Graphics::White, Graphics::TriangleFan};
    WingStyle.thickness = 2.0f;
    WingStyle.lineColor = Graphics::FColor::FromBytes(250, 116, 42)*1.5;
    WingStyle.fillColor = Graphics::FColor::FromBytes(250, 116, 42, 128);

    for (const auto &Wing : Wings) {

        auto ProfileVertices
        = Wing->Airfoil->GetProfileVertices(DefaultAirfoilProfileNumSegments, Wing->Params.ChordLength, Wing->Params.ThicknessInUnitsOfChortLength);

        const auto Body = Wing->BodyId;
        const auto [x, y] = b2Body_GetPosition(Body);
        const auto [cosWing, sinWing] = b2Body_GetRotation(Body);
        for (auto &Point : ProfileVertices.GetPoints()) {
            fix px = Point.x;
            fix py = Point.y;

            Point.x = x + px*cosWing - py*sinWing;
            Point.y = y + px*sinWing + py*cosWing;
        }

        Drawer::RenderPointSet(Dummy(ProfileVertices), WingStyle);

    }

    if constexpr (false) {
        Graphics::PlotStyle HullStyle{Graphics::LightGrey, Graphics::LineLoop};

        WingStyle.thickness = 2.0f;
        WingStyle.lineColor.a = 1.0f;
        fix ShapeCount = b2Body_GetShapeCount(HullBody);
        Vector<b2ShapeId> Shapes(ShapeCount);
        b2Body_GetShapes(HullBody, &Shapes[0], ShapeCount);
        for (const auto &Shape : Shapes) {
            const b2ShapeType Type = b2Shape_GetType(Shape);
            if (Type == b2_polygonShape) {
                const b2Polygon Poly = b2Shape_GetPolygon(Shape);

                Math::FPointSet Points;

                for (int i=0; i<Poly.count; ++i) {
                    fix &[x,y] = Poly.vertices[i];
                    Points.AddPoint({xBody + x*c - y*s,
                                     yBody + x*s + y*c });
                }

                Drawer::RenderPointSet(Dummy(Points), HullStyle);
            }
        }
    }
}

void FLittlePlane::AdjustWingAngle(int WingId, double Delta) const {
    if (WingId >= Wings.size()) return;

    const auto &Wing = Wings[WingId];

    Wing->SetBaseAngle(Delta);
}

FLittlePlane::FLittlePlane(const Vector<TPointer<FWing>>& Wings, const b2BodyId HullBody): Wings(Wings), HullBody(HullBody) {
    fix Loc = Core::Resources::GetResourcesPath() + "/LittlePlaneDesigner/Extra300S_03.png";
    auto HullTextureResult = Graphics::OpenGL::FTexture2D_Color::FromImageFile(Loc);
    if (HullTextureResult.IsFailure()) {
        Core::Log::Error("Failed to load texture: " + Loc) << HullTextureResult.ToString() << Core::Log::Flush;
        throw std::runtime_error("Failed to load texture: " + Loc);
    }

    HullTexture = HullTextureResult.Value();
}
