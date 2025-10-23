//
// Created by joao on 10/16/25.
//

#include "Foil.h"

#include "3rdParty/ImGui.h"

Foil::FAirfoilDynamicData Foil::ComputeAirfoilForces(const IAirfoil& Airfoil, const b2BodyId& Body,
                                                     const LegacyGLDebugDraw& DebugDraw_LegacyGL) {
    // Geometry
    constexpr b2Vec2 LE_local = {-0.25f, 0.0f};
    constexpr b2Vec2 QuarterChordLocal = LE_local;
    // const b2Vec2 QuarterChordWorld = b2Body_GetWorldPoint(Body, QuarterChordLocal);

    // Directions
    // const b2Vec2 AirfoilForwardWorldUnit = b2Body_GetWorldVector(Body, b2Vec2(-1.0f, 0.0f));

    // Kinematics at c/4: use WORLD CENTER, not position
    // const b2Vec2 COM = b2Body_GetWorldCenterOfMass(Body);
    // const b2Vec2 LinearSpeed = b2Body_GetLinearVelocity(Body);
    const float  ω    = b2Body_GetAngularVelocity(Body);
    // const b2Vec2 R    = QuarterChordWorld - COM;
    // const b2Vec2 v_point = LinearSpeed + b2Vec2(-ω * R.y, ω * R.x);

    const float theta = b2Rot_GetAngle(b2Body_GetRotation(Body));
    ImGui::Begin("Airfoil");
    ImGui::Text("theta: %.2f", theta/M_PI*180.0);
    const b2Vec2 v_point = b2Body_GetLocalPointVelocity(Body, QuarterChordLocal);
    const float AoA = atan2(-v_point.y, v_point.x) + theta; // AoA
    ImGui::Text("AoA: %.2f", AoA/M_PI*180);
    ImGui::End();

    // Wind
    const b2Vec2 WindOnPoint = -v_point;
    const float  WindOnPointLen    = b2Length(WindOnPoint);
    if (WindOnPointLen < 1e-6f) return FAirfoilDynamicData::Null();
    const b2Vec2 WindOnPointUnit = b2Normalize(WindOnPoint); // TODO: could take advantage of already computed length above

    // AoA
    // const auto AirfoilNormalUnit = perpCW(AirfoilForwardWorldUnit);
    // const double Cos = std::clamp<double>(AirfoilNormalUnit.x*WindOnPointUnit.x + AirfoilNormalUnit.y*WindOnPointUnit.y, -1.0, 1.0);
    // const double Sin = static_cast<double>(AirfoilNormalUnit.x)*WindOnPointUnit.y - static_cast<double>(AirfoilNormalUnit.y)*WindOnPointUnit.x;
    // const double AoA = std::atan2(Sin, Cos) + M_PI_2;

    // Coeffs
    const double Cl = Airfoil.Cl(AoA);
    const double Cd = Airfoil.Cd(AoA);
    const double Cm_c4 = Airfoil.Cm_c4(AoA);

    // Params:
    constexpr auto rho = 1.250;
    constexpr auto chord_length = 1.0;
    constexpr auto span = 0.5;

    // Dynamic pressure (keep density sane)
    const double q   = 0.5 * rho * static_cast<double>(WindOnPointLen) * static_cast<double>(WindOnPointLen);
    const double S   = chord_length * span;

    // Magnitudes
    const double Lmag = q * S * Cl;
    const double Dmag = q * S * Cd;
    double Tmag = q * span * chord_length * chord_length * Cm_c4;

    // Simple aero angular damping to suppress runaway spin
    // const double c_rot = 0.05 * q * S * P.ChordLength;          // tune
    // Tmag -= c_rot * static_cast<double>(ω);

    // Forces
    const b2Vec2 drag = -static_cast<float>(-Dmag) * WindOnPointUnit;
    const b2Vec2 lift = +static_cast<float>(+Lmag) * b2Vec2(-WindOnPointUnit.y, WindOnPointUnit.x);

    const b2Vec2 QuarterChordWorld = b2Body_GetWorldPoint(Body, QuarterChordLocal);
    {
        const b2Vec2 COM = b2Body_GetWorldCenterOfMass(Body);
        const b2Vec2 COMSpeed = b2Body_GetLinearVelocity(Body);

        DebugDraw_LegacyGL.DrawVector(COMSpeed, COM, .25f, b2_colorCadetBlue);
        DebugDraw_LegacyGL.Write("COM speed", COM + COMSpeed*.25f, b2_colorCadetBlue);

        {
            constexpr auto lift_scale = 0.1f;
            constexpr auto drag_scale = 0.1f;

            DebugDraw_LegacyGL.DrawVector(v_point, QuarterChordWorld, .25f, b2_colorAliceBlue);
            DebugDraw_LegacyGL.Write(Str("C/4 speed @ " + ToStr(AoA/M_PI*180.0) + "deg AoA"), QuarterChordWorld + v_point*.225f, b2_colorAliceBlue);

            DebugDraw_LegacyGL.DrawPseudoVector(ω, COM, .25f, 0.25f*M_PI, b2_colorRed);
            DebugDraw_LegacyGL.Write("Ang speed", COM+b2Vec2{ω*.26f, .0f}, b2_colorRed);

            DebugDraw_LegacyGL.DrawVector(WindOnPoint, QuarterChordWorld, 0.1f, b2_colorCadetBlue);
            DebugDraw_LegacyGL.Write("Wind", QuarterChordWorld + WindOnPoint*0.1f, b2_colorCadetBlue);

            // DebugDraw_LegacyGL.DrawVector(AirfoilNormalUnit, c4_world, 1.f, b2_colorDarkBlue);
            // DebugDraw_LegacyGL.Write(Str("Airfoil normal @ " + ToStr(AoA/M_PI*180.0) + "deg AoA"), c4_world + AirfoilNormalUnit, b2_colorDarkBlue);

            {
                const b2Vec2 AirfoilForwardWorldUnit = b2Body_GetWorldVector(Body, b2Vec2(-1.0f, 0.0f));
                DebugDraw_LegacyGL.DrawVector(AirfoilForwardWorldUnit, QuarterChordWorld, 1.f, b2_colorDarkBlue);
                DebugDraw_LegacyGL.Write("Forward", QuarterChordWorld + AirfoilForwardWorldUnit, b2_colorDarkBlue);
            }

            DebugDraw_LegacyGL.DrawVector(drag, QuarterChordWorld, drag_scale, b2_colorRed);
            DebugDraw_LegacyGL.Write("drag", QuarterChordWorld + drag*drag_scale, b2_colorRed);

            DebugDraw_LegacyGL.DrawVector(lift, QuarterChordWorld, lift_scale, b2_colorAliceBlue);
            DebugDraw_LegacyGL.Write("lift", QuarterChordWorld + lift*lift_scale, b2_colorAliceBlue);

            DebugDraw_LegacyGL.DrawVector(lift+drag, QuarterChordWorld, .05f, b2_colorAliceBlue);
            DebugDraw_LegacyGL.Write("lift+drag ", QuarterChordWorld + (lift+drag)*.05f, b2_colorAliceBlue);

            DebugDraw_LegacyGL.DrawPseudoVector(Tmag, COM, 1.f, b2_colorBisque);
            DebugDraw_LegacyGL.Write("Torque@c/4", COM+b2Vec2{static_cast<float>(Tmag), .0f}, b2_colorBisque);
        }
    }

    return FAirfoilDynamicData{
        drag, lift, QuarterChordWorld,
        static_cast<float>(Tmag),
        static_cast<float>(Cl),
        static_cast<float>(Cd),
        static_cast<float>(Cm_c4),
        static_cast<float>(AoA)};

}