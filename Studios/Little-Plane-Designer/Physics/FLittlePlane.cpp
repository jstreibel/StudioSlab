//
// Created by joao on 10/25/25.
//

#include "FLittlePlane.h"

#include <SFML/Window/Keyboard.hpp>

#include "FAtmosphericCondition.h"
#include "Foils/Foil.h"
#include "Graphics/OpenGL/LegacyGL/PointSetRenderer.h"
#include "Graphics/Plot2D/PlotStyle.h"

Foil::FAirfoilDynamicData FLittlePlane::ComputeForces(
    const FWing& Wing,
    const FAtmosphericCondition &Atmosphere,
    const TPointer<LegacyGLDebugDraw>& DebugDraw)
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

    if (DebugDraw != nullptr)
    {
        const auto &DebugDraw_LegacyGL = *DebugDraw;

        const auto Scale = DebugDraw_LegacyGL.GetBaseScale();

        const auto Total = drag + lift;
        DebugDraw_LegacyGL.DrawVector(Total, COM, 1.0f, b2_colorRed);
        DebugDraw_LegacyGL.Write("Total", COM + Total*Scale, b2_colorRed);

        DebugDraw_LegacyGL.DrawVector(drag, COM, 1.0f, b2_colorRed);
        DebugDraw_LegacyGL.Write("drag", COM + drag*Scale, b2_colorRed);

        DebugDraw_LegacyGL.DrawVector(lift, COM, 1.0f, b2_colorAliceBlue);
        DebugDraw_LegacyGL.Write("lift", COM + lift*Scale, b2_colorAliceBlue);

        if constexpr (false)
        {
            DebugDraw_LegacyGL.DrawPseudoVector(ω, COM, 1.f, .0f, b2_colorAqua);
            DebugDraw_LegacyGL.Write("omega", COM+b2Vec2{static_cast<float>(ω), .0f}, b2_colorAqua);

            DebugDraw_LegacyGL.DrawPseudoVector(τ_mag, COM, 1.f, .0f, b2_colorBisque);
            DebugDraw_LegacyGL.Write("Torque Cm", COM+b2Vec2{static_cast<float>(τ_mag), .0f}, b2_colorBisque);

            DebugDraw_LegacyGL.DrawPseudoVector(τ_aero, COM, 1.f, .0f, b2_colorDarkCyan);
            DebugDraw_LegacyGL.Write("Torque (aero)", COM+b2Vec2{static_cast<float>(τ_aero), .0f}, b2_colorDarkCyan);

            DebugDraw_LegacyGL.DrawPseudoVector(τ_visc, COM, 1.f, .0f, b2_colorBox2DBlue);
            DebugDraw_LegacyGL.Write("Torque (visc)", COM+b2Vec2{static_cast<float>(τ_visc), .0f}, b2_colorBox2DBlue);
        }
    }

    return Foil::FAirfoilDynamicData{
        drag, lift, COM,
        static_cast<float>(τ),
        static_cast<float>(Cl),
        static_cast<float>(Cd),
        static_cast<float>(Cm_c4),
        static_cast<float>(AoA)};
}

void FLittlePlane::ComputeAndApplyForces(const TPointer<LegacyGLDebugDraw> DebugDraw) {
    for (auto &Wing : Wings) {
        const auto &WingBody = Wing->BodyId;
        if (!b2Body_IsAwake(WingBody)) continue;

        const auto CurrentForces = ComputeForces(*Wing, {}, DebugDraw);
        b2Body_ApplyForce (WingBody, CurrentForces.GetTotalForce(), CurrentForces.loc, true);
        b2Body_ApplyTorque(WingBody, CurrentForces.torque, true);
    }
}

float FLittlePlane::GetTotalMass() const {
    float Mass = 0.0f;
    for (const auto &Wing : Wings) {
        Mass += b2Body_GetMass(Wing->BodyId);
    }

    Mass += b2Body_GetMass(HullBody);

    return Mass;
}

b2Vec2 FLittlePlane::GetCenterOfMass_Global() const {
    fix HullCOM = b2Body_GetWorldCenterOfMass(HullBody);
    fix HullMass = b2Body_GetMass(HullBody);

    b2Vec2 COM_Accumulator = HullMass * HullCOM;
    auto TotalMass = HullMass;

    for (const auto &Wing : Wings) {
        fix WingCOM = b2Body_GetWorldCenterOfMass(Wing->BodyId);
        fix WingMass = b2Body_GetMass(Wing->BodyId);

        COM_Accumulator += WingMass * WingCOM;
        TotalMass += WingMass;
    }

    return COM_Accumulator * (1.f/TotalMass);
}

b2Vec2 FLittlePlane::GetPosition() const { return b2Body_GetPosition(HullBody); }

b2Vec2 FLittlePlane::GetVelocity() const {
    return b2Body_GetLinearVelocity(HullBody);
}

void FLittlePlane::Draw(const Graphics::FPlatformWindow&) {
    namespace Drawer = Graphics::OpenGL::Legacy;

    Graphics::PlotStyle WingStyle{Graphics::White, Graphics::TriangleFan};
    WingStyle.thickness = 2.0f;
    WingStyle.lineColor.a = 1.0f;

    for (const auto &Wing : Wings) {
        const Math::FPointSet AirfoilPoints = Wing->Airfoil->GetProfileVertices(200, Wing->Params.ChordLength, Wing->Params.Thickness);
        Math::FPointSet Points = AirfoilPoints;

        const auto Body = Wing->BodyId;
        const auto [x, y] = b2Body_GetPosition(Body);
        const auto [c, s] = b2Body_GetRotation(Body);
        for (auto &Point : Points.getPoints()) {
            const auto chord = Wing->Params.ChordLength;
            fix px = Point.x -chord*.5f;
            fix py = Point.y;

            Point.x = x + px*c - py*s;
            Point.y = y + px*s + py*c;
        }

        Drawer::RenderPointSet(Dummy(Points), WingStyle);
    }
}

void FLittlePlane::AdjustWingAngle(int WingId, double Delta) const {
    if (WingId >= Wings.size()) return;

    const auto &Wing = Wings[WingId];

    Wing->SetBaseAngle(Delta);
}

