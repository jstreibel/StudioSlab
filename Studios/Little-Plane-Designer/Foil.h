//
// Created by joao on 10/16/25.
//

#ifndef STUDIOSLAB_FOIL_H
#define STUDIOSLAB_FOIL_H

#include "box2d/box2d.h"
#include "Math/Point.h"
#include "Math/VectorSpace/Impl/PointSet.h"
#include "Utils/String.h"
#include "Utils/Arrays.h"

using namespace Slab;

namespace Foil
{

class IAirfoil
{
    public:
    virtual ~IAirfoil() = default;

    virtual Str GetName() const = 0;

    virtual Math::PointSet GetProfileVertices(int N = 200) const = 0;

    virtual double Cl(double AlphaRad) const = 0;
    virtual double Cd(double AlphaRad) const = 0;
    virtual double Cm_c4(double AlphaRad) const = 0;

};

class Airfoil_NACA2412 : public IAirfoil
{
public:
  // ---- Polars ----
  double Cl(const double alpha_rad) const override {
    double cl = a * (alpha_rad - alpha0);
    if (cl > cl_max) cl = cl_max;
    if (cl < cl_min) cl = cl_min;
    return cl;
  }
  double Cd(const double alpha_rad) const override {
    const double cl = Cl(alpha_rad);
    return cd0 + k * cl * cl;
  }
  double Cm_c4(double /*alpha_rad*/) const override { return cm_c4; } // approx. constant

  // Return a single polyline: start at TE on lower surface,
  // go to LE, then back to TE on upper surface. N+1 points per side.
  Math::PointSet GetProfileVertices(int N = 200) const override {
    if (N < 2) N = 2;
    Math::Point2DVec v;
    v.reserve(2 * (N + 1));

    // Lower surface from TE->LE
    for (int i = 0; i <= N; ++i) {
      const double x = 1.0 - static_cast<double>(i) / N; // 1..0
      const double th = std::atan(dyc_dx(x));
      const double yt_ = yt(x);
      const double yc_ = yc(x);
      double xl = x + yt_ * std::sin(th);
      double yl = yc_ - yt_ * std::cos(th);
      v.push_back({xl, yl});
    }
    // Upper surface from LE->TE
    for (int i = 0; i <= N; ++i) {
      const double x = static_cast<double>(i) / N; // 0..1
      const double th = std::atan(dyc_dx(x));
      const double yt_ = yt(x);
      const double yc_ = yc(x);
      double xu = x - yt_ * std::sin(th);
      double yu = yc_ + yt_ * std::cos(th);
      v.push_back({xu, yu});
    }
    return Math::PointSet(v);
  }

    Str GetName() const override { return "NACA2412"; }

private:
    // NACA m=camber, p=camber pos, t=thickness (unit chord)
    double m = 0.02;     // 2412
    double p = 0.40;
    double t = 0.12;

    // Polar params (pre-stall)
    double a      = 2.0 * M_PI; // lift slope [1/rad]
    double alpha0 = -0.038;     // zero-lift angle [rad]
    double cl_max = 1.4;        // +stall
    double cl_min = -1.2;       // -stall
    double cd0    = 0.008;      // profile drag @ Cl≈0
    double k      = 0.010;      // induced-like term
    double cm_c4  = -0.05;      // pitching moment about c/4

    static double deg2rad(const double deg) { return deg * M_PI / 180.0; }

    // ---- Geometry helpers ----
    // NACA 4-digit formulas on x∈[0,1]
    double yt(double x) const {
        const double rt = std::sqrt(std::max(0.0, x));
        return 5.0 * t * (0.2969 * rt - 0.1260 * x - 0.3516 * x * x
                          + 0.2843 * x * x * x - 0.1015 * x * x * x * x);
    }
    double yc(double x) const {
        if (x < p) return (m / (p * p)) * (2.0 * p * x - x * x);
        return (m / ((1.0 - p) * (1.0 - p))) * ((1.0 - 2.0 * p) + 2.0 * p * x - x * x);
    }
    double dyc_dx(double x) const {
        if (x < p) return 2.0 * m / (p * p) * (p - x);
        return  2.0 * m / ((1.0 - p) * (1.0 - p)) * (p - x);
    }
};

struct FAeroParams {
    double chord = 1.0;     // meters
    double span  = 0.1;     // meters (effective 2D "depth")
    double rho   = 1.225;   // kg/m^3
    // local geometry: LE at x=0, chord along +x in body local frame

    // b2Vec2 le_local = {0.0f, 0.0f}; // leading-edge local point
    b2Vec2 le_local = {-0.25f/* x chord */, 0.0f}; // leading-edge local point
};

// Rotate +90° helper
static b2Vec2 perpCCW(const b2Vec2& v) { return b2Vec2(-v.y, v.x); }

static float len(const b2Vec2& v) { return std::sqrt(v.x*v.x + v.y*v.y); }

static b2Vec2 nrm(const b2Vec2& v) {
    const float L = len(v);
    return L > 0 ? 1.0f/L * v : b2Vec2(0,0);
}

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
inline void ApplyAirfoilForces(const IAirfoil& Airfoil, const b2BodyId& Body, const FAeroParams& P)
{
    // Geometry
    const float c = (float)P.chord;
    const b2Vec2 c4_local = P.le_local + b2Vec2(0.25f * c, 0.0f);
    const b2Vec2 c4_world = b2Body_GetWorldPoint(Body, c4_local);

    // Directions
    const b2Vec2 t_world = b2Body_GetWorldVector(Body, b2Vec2(1.0f, 0.0f));
    const float tlen = std::sqrt(t_world.x*t_world.x + t_world.y*t_world.y);
    if (tlen <= 0.0f) return;
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
    if (V < 1e-3f) return;
    const b2Vec2 w_hat = (1.0f / V) * wind;

    // AoA
    const double dot = std::clamp<double>(chord_hat.x*w_hat.x + chord_hat.y*w_hat.y, -1.0, 1.0);
    const double det = (double)chord_hat.x*w_hat.y - (double)chord_hat.y*w_hat.x;
    const double aoa = -std::atan2(det, dot);

    // Coeffs
    double Cl = Airfoil.Cl(aoa);
    double Cd = Airfoil.Cd(aoa);
    double Cm = Airfoil.Cm_c4(aoa);

    // Dynamic pressure (keep density sane)
    const double rho = std::clamp(P.rho, 0.1, 5.0);       // guard
    const double q   = 0.5 * rho * (double)V * (double)V;
    const double S   = P.chord * P.span;

    // Magnitudes
    const double Lmag = q * S * Cl;
    const double Dmag = q * S * Cd;
    double Tmag = q * P.span * P.chord * P.chord * Cm;

    // Simple aero angular damping to suppress runaway spin
    const double c_rot = 0.05 * q * S * P.chord;          // tune
    Tmag -= c_rot * (double)w;

    // Optional caps for stability
    const double Fcap = 50.0 * S;                         // tune to your mass scale
    const double Tcap = 50.0 * S * P.chord;
    const double Lm = std::clamp(Lmag, -Fcap, Fcap);
    const double Dm = std::clamp(Dmag, -Fcap, Fcap);
    Tmag = std::clamp(Tmag, -Tcap, Tcap);

    // Forces
    const b2Vec2 drag = (float)(-Dm) * w_hat;
    const b2Vec2 lift = (float)( Lm) * b2Vec2(-w_hat.y, w_hat.x);
    const b2Vec2 F    = drag + lift;

    // Apply at c/4
    b2Body_ApplyForce(Body, F, c4_world, true);
    b2Body_ApplyTorque(Body, (float)Tmag, true);
}
}


#endif //STUDIOSLAB_FOIL_H