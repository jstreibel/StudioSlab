//
// Created by joao on 10/25/25.
//

#include "FLittlePlane.h"

#include <SFML/Window/Keyboard.hpp>

#include "Foils/Foil.h"

constexpr b2Vec2 x0{2.5f, 1.5f};
constexpr auto α0 = .0f; // 0.3f;
constexpr auto ω0 = .0f; // 0.8354123f;
constexpr b2Vec2 v0{-5.f, 0.0f};

FLittlePlane::FLittlePlane() {
    void SetupWing();
}

void FLittlePlane::SetupWing(
    const b2WorldId World,
    const TPointer<Foil::IAirfoilPolars>& Airfoil,
    const Foil::FAirfoilParams& Params) {
    // Dynamic box
    b2BodyDef bodyDef = b2DefaultBodyDef();
    bodyDef.type = b2_dynamicBody;
    // bodyDef.position = (b2Vec2){ViewSize*(.5f), ViewSize*.25f};
    bodyDef.position = x0;
    bodyDef.rotation =  b2MakeRot(α0);
    bodyDef.angularVelocity = ω0;
    bodyDef.linearVelocity = v0;

    auto WingBody = b2CreateBody(World, &bodyDef);
    b2Body_SetName(WingBody, Params.Name.c_str());
    FWing Wing = {
        .BodyId = WingBody,
        .Airfoil = Airfoil,
        .Params = Params
    };
    Wings.emplace_back(WingBody, Airfoil, Params);

    // Rectangle with c/4 at (0,0):
    // LE=-0.25c, TE=+0.75c

    fix Chord = Params.ChordLength;
    fix Thick = Params.Thickness;

    // 1) Build hull
    b2Hull hull;
    if constexpr (false)
    {
        constexpr int N = B2_MAX_POLYGON_VERTICES;
        const auto AirfoilPoints = Airfoil->GetProfileVertices(N).getPoints();
        b2Vec2 pts[N];
        for (int i=0; i<N; ++i) {
            pts[i].x = AirfoilPoints[i].x;
            pts[i].y = AirfoilPoints[i].y;
        }
        hull = b2ComputeHull(pts, N);
    }
    else
    {
        const b2Vec2 pts[4] = {
            { -0.5f*Chord, -0.5f*Thick },
            {  0.5f*Chord, -0.5f*Thick },
            {  0.5f*Chord,  0.5f*Thick },
            { -0.5f*Chord,  0.5f*Thick }
        };
        hull = b2ComputeHull(pts, 4);
    }

    // 2) Make convex polygon from hull
    const b2Polygon WingShape = b2MakePolygon(&hull, 0.0f);          // if your API needs radius: b2MakePolygon(&hull, 0.0f)

    // 3) Create a fixture
    b2ShapeDef sdef = b2DefaultShapeDef();
    sdef.density = 1.0f;
    sdef.material.friction = 0.1f;
    b2CreatePolygonShape(WingBody, &sdef, &WingShape);

    auto COM = Params.COM;
    ShiftBodyCOM(COM.x*Chord, COM.y*Thick, WingBody); // 38% behind LE (NACA2412 usually sits 35-40% chord length)
}

Foil::FAirfoilDynamicData FLittlePlane::ComputeForces(
    const FWing& Wing,
    const TPointer<LegacyGLDebugDraw>& DebugDraw)
{
    // Directions
    const b2Vec2 FW_Unit = b2Body_GetWorldVector(Wing.BodyId, b2Vec2(-1.0f, 0.0f));

    // Kinematics at c/4: use WORLD CENTER, not position
    const b2Vec2 COM = b2Body_GetWorldCenterOfMass(Wing.BodyId);
    const b2Vec2 VelC4 = b2Body_GetLinearVelocity(Wing.BodyId);// const float QuarterChordVelocity_Mag = b2Length(QuarterChordVelocity);
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
    const auto rho = Wing.Params.rho;
    const auto chord_length = Wing.Params.ChordLength;
    const auto span = Wing.Params.Span;

    constexpr struct RotDampParams {
        float Cmq    = -0.1f;   // nondimensional pitch-rate derivative
        float Komega = 0.1f;     // extra viscous damping [N·m·s] per unit span
    } P;

    // Dynamic pressure (keep density sane)
    const double q   = 0.5 * rho * static_cast<double>(WindOnPointLen) * static_cast<double>(WindOnPointLen);
    const double S   = chord_length * span;

    // Magnitudes
    const double Lmag = q * S * Cl;
    const double Dmag = q * S * Cd;
    const double τ_mag = 0.5 * q * span * chord_length * chord_length * Cm_c4;
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

void FLittlePlane::ShiftBodyCOM(const float Δx, const float Δy, const b2BodyId Body) {
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

