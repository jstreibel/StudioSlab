//
// Created by joao on 10/25/25.
//

#include "FLittlePlane.h"

#include <SFML/Window/Keyboard.hpp>

#include "FAtmosphericCondition.h"
#include "Foils/Foil.h"

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
        DebugDraw_LegacyGL.DrawVector(VelC4, COM, 1.f, b2_colorAquamarine);
        DebugDraw_LegacyGL.Write("speed", VelC4 + COM, b2_colorAquamarine);

        {
            constexpr auto lift_scale = 1.f;
            constexpr auto drag_scale = 1.f;

            // DebugDraw_LegacyGL.DrawVector(VelC4, COM + QuarterChordWorld, .25f, b2_colorAliceBlue);
            // DebugDraw_LegacyGL.Write(Str("C/4 speed @ " + ToStr(AoA/M_PI*180.0) + "deg AoA"), QuarterChordWorld + VelC4*.225f, b2_colorAliceBlue);

            // DebugDraw_LegacyGL.DrawPseudoVector(ω, COM, .25f, 0.25f*M_PI, b2_colorRed);
            // DebugDraw_LegacyGL.Write("Ang speed", COM+b2Vec2{ω*.26f, .0f}, b2_colorRed);

            // DebugDraw_LegacyGL.DrawVector(WindOnPoint, COM, 0.1f, b2_colorCadetBlue);
            // DebugDraw_LegacyGL.Write("Wind", COM + WindOnPoint*0.1f, b2_colorCadetBlue);

            // DebugDraw_LegacyGL.DrawVector(AirfoilNormalUnit, c4_world, 1.f, b2_colorDarkBlue);
            // DebugDraw_LegacyGL.Write(Str("Airfoil normal @ " + ToStr(AoA/M_PI*180.0) + "deg AoA"), c4_world + AirfoilNormalUnit, b2_colorDarkBlue);

            // DebugDraw_LegacyGL.DrawVector(FW_Unit, QuarterChordWorld, 1.f, b2_colorDarkBlue);
            // DebugDraw_LegacyGL.Write("Forward", QuarterChordWorld + FW_Unit, b2_colorDarkBlue);

            DebugDraw_LegacyGL.DrawVector(drag, COM, drag_scale, b2_colorRed);
            DebugDraw_LegacyGL.Write("drag", COM + drag*drag_scale, b2_colorRed);

            DebugDraw_LegacyGL.DrawVector(lift, COM, lift_scale, b2_colorAliceBlue);
            DebugDraw_LegacyGL.Write("lift", COM + lift*lift_scale, b2_colorAliceBlue);

            // DebugDraw_LegacyGL.DrawVector(lift+drag, COM, .05f, b2_colorAliceBlue);
            // DebugDraw_LegacyGL.Write("lift+drag ", COM + (lift+drag)*.05f, b2_colorAliceBlue);

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

