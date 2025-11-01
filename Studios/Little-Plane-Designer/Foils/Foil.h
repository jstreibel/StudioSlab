//
// Created by joao on 10/16/25.
//

#ifndef STUDIOSLAB_FOIL_H
#define STUDIOSLAB_FOIL_H

#include "../DebugDraw.h"
#include "box2d/box2d.h"
#include "Math/VectorSpace/Impl/PointSet.h"
#include "Utils/RandUtils.h"
#include "Utils/String.h"

using namespace Slab;

namespace Foil
{

struct FAirfoilParams {
    const Str Name = ToStr(RandUtils::RandomUniformUInt());

    const float ChordLength = 1.0f;
    const float Thickness  = 0.1f;
    const float Span  = 0.5f;
    const float rho   = 1.225f;
    const float LE_local = -0.25f; // from COM
    // COM from geometric center, in units of chord_length and thickness, respectively
    // Hint: 38% behind LE (NACA2412 usually sits 35-40% chord length)
    const b2Vec2 COM = b2Vec2{-.25f, -0.01f};
    // Local revolute joint anchor, relative to COM
    const b2Vec2 LocalAnchor = {.0f, .0f};
};

class IAirfoilPolars
{
    public:
    virtual ~IAirfoilPolars() = default;

    virtual Str GetName() const = 0;

    virtual Math::PointSet GetProfileVertices(int N, float chord_length, float thickness) const = 0;

    virtual double Cl(double AlphaRad) const = 0;
    virtual double Cd(double AlphaRad) const = 0;
    /**
     * $C_{m,c/4}$ is the pitching-moment coefficient about the quarter-chord point,
     *  not the overall about an arbitrary reference.
     * @param AlphaRad
     * @return
     */
    virtual double Cm_c4(double AlphaRad) const = 0;

};

// Rotate +90° helper
static b2Vec2 perpCCW(const b2Vec2& v) { return b2Vec2(-v.y, v.x); }

// Rotate -90° helper
static b2Vec2 perpCW(const b2Vec2& v) { return b2Vec2(v.y, -v.x); }

// Signed AoA between chord direction and incoming wind
static double signedAoA(const b2Vec2& chord_hat, const b2Vec2& wind_hat) {
    // chord to wind angle (positive if wind is above chord, CCW)
    const double dot = std::clamp<double>(chord_hat.x*wind_hat.x + chord_hat.y*wind_hat.y, -1.0, 1.0);
    const double det = chord_hat.x*wind_hat.y - chord_hat.y*wind_hat.x; // 2D cross (z-component)
    const double ang = std::atan2(det, dot); // angle from chord to wind
    // AoA is negative of that (AoA measured from incoming flow to chord)
    return -ang;
}

// Apply aero forces and moment at quarter-chord using Box2D C API.
// Requires: IAirfoil { double Cl(double), Cd(double), Cm_c4(double) }.
// Params: chord in meters, span effective depth, rho density, le_local leading edge in body-local coords.
struct FAirfoilDynamicData {
    const b2Vec2 drag, lift, loc;
    const float torque;
    const float Cl, Cd, Cm_c4;
    const float AoA;
    b2Vec2 GetTotalForce() const { return drag+lift; }
    static auto Null() { return FAirfoilDynamicData{b2Vec2(0.0f, 0.0f), b2Vec2(0.0f, 0.0f), b2Vec2(0.0f, 0.0f), 0.0f}; }
};

inline Graphics::Point2D ToPoint2D(const b2Vec2& v) { return Graphics::Point2D{v.x, v.y}; }

inline FAirfoilDynamicData ComputeAirfoilForces2(
    const IAirfoilPolars& Airfoil,
    const b2BodyId& Body,
    const LegacyGLDebugDraw &DebugDraw_LegacyGL)
{
    // Params:
    constexpr auto rho = 1.250;
    constexpr auto chord_length = 1.0;
    constexpr auto span = 0.5;
    constexpr b2Vec2 LE_local = {-0.25f, 0.0f};

    // Geometry
    const float c = (float)chord_length;
    const b2Vec2 c4_local = LE_local + b2Vec2(0.25f * c, 0.0f);
    const b2Vec2 c4_world = b2Body_GetWorldPoint(Body, c4_local);

    // Directions
    const b2Vec2 t_world = b2Body_GetWorldVector(Body, b2Vec2(1.0f, 0.0f));
    const float tlen = std::sqrt(t_world.x*t_world.x + t_world.y*t_world.y);
    if (tlen <= 0.0f) return FAirfoilDynamicData::Null();
    const b2Vec2 chord_hat = (1.0f / tlen) * t_world;

    // Kinematics at c/4: use WORLD CENTER, not position
    const b2Vec2 com = b2Body_GetWorldCenterOfMass(Body);       // <- key fix
    const b2Vec2 vlin = b2Body_GetLinearVelocity(Body);
    const float  w    = b2Body_GetAngularVelocity(Body);
    const b2Vec2 r    = c4_world - com;
    const b2Vec2 v_point = vlin + b2Vec2(-w * r.y, w * r.x);

    // Wind
    const b2Vec2 wind = -v_point;
    const float  V    = std::sqrt(wind.x*wind.x + wind.y*wind.y);
    if (V < 1e-3f) return FAirfoilDynamicData::Null();
    const b2Vec2 w_hat = (1.0f / V) * wind;
    DebugDraw_LegacyGL.DrawVector(wind, c4_world, 0.1f, b2_colorHotPink);
    DebugDraw_LegacyGL.Write("Wind direction", c4_world + wind*0.1f);

    // AoA
    const double dot = std::clamp<double>(chord_hat.x*w_hat.x + chord_hat.y*w_hat.y, -1.0, 1.0);
    const double det = (double)chord_hat.x*w_hat.y - (double)chord_hat.y*w_hat.x;
    const double aoa = -std::atan2(det, dot);

    // Coeffs
    double Cl = Airfoil.Cl(aoa);
    double Cd = Airfoil.Cd(aoa);
    double Cm = Airfoil.Cm_c4(aoa);

    // Dynamic pressure (keep density sane)
    const double q   = 0.5 * rho * (double)V * (double)V;
    const double S   = chord_length * span;

    // Magnitudes
    const double Lmag = q * S * Cl;
    const double Dmag = q * S * Cd;
    double Tmag = q * span * chord_length * chord_length * Cm;

    // Simple aero angular damping to suppress runaway spin
    const double c_rot = 0.05 * q * S * chord_length;          // tune
    Tmag -= c_rot * (double)w;

    // Optional caps for stability
    const double Fcap = 50.0 * S;                         // tune to your mass scale
    const double Tcap = 50.0 * S * chord_length;
    const double Lm = std::clamp(Lmag, -Fcap, Fcap);
    const double Dm = std::clamp(Dmag, -Fcap, Fcap);
    Tmag = std::clamp(Tmag, -Tcap, Tcap);

    // Forces
    const b2Vec2 drag = -(float)(-Dm) * w_hat;
    const b2Vec2 lift = (float)( Lm) * b2Vec2(-w_hat.y, w_hat.x);

    constexpr auto drag_scale = 10.0f;
    DebugDraw_LegacyGL.DrawVector(drag, c4_world, drag_scale, b2_colorRed);
    DebugDraw_LegacyGL.Write("drag", c4_world + drag*drag_scale, b2_colorRed);
    DebugDraw_LegacyGL.DrawVector(lift, c4_world, 1.f, b2_colorAliceBlue);
    DebugDraw_LegacyGL.Write("lift", c4_world + lift, b2_colorAliceBlue);
    DebugDraw_LegacyGL.DrawPseudoVector(Tmag, com);

    return FAirfoilDynamicData{drag, lift, c4_world, (float)Tmag};
}
}


#endif //STUDIOSLAB_FOIL_H