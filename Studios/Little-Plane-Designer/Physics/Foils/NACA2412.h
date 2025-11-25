//
// Created by joao on 10/20/25.
//

#ifndef STUDIOSLAB_NACA2412_H
#define STUDIOSLAB_NACA2412_H

#include "Foil.h"

namespace Foil {

class Airfoil_NACA2412 final : public IAirfoilPolars
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
    Math::FPointSet GetProfileVertices(int N, float chord_length, float thickness_in_units_of_chord) const override;

    Str GetName() const override { return "NACA2412"; }

    /*
    struct FAeroParams {
        const double ChordLength = 1.0;     // meters
        const double span  = 1.0;     // meters (effective 2D "depth")
        const double rho   = 1.225;   // kg/m^3
        // local geometry: LE at x=0, chord along +x in body local frame

        // b2Vec2 le_local = {0.0f, 0.0f}; // leading-edge local point
        const b2Vec2 LE_local = {-0.25f, 0.0f}; // leading-edge local point
    } Params;*/

private:
    // Polar params (pre-stall)
    const double a      = 2.0 * M_PI; // lift slope [1/rad]
    const double alpha0 = -0.038;     // zero-lift angle [rad]
    const double cl_max = 1.4;        // +stall
    const double cl_min = -1.2;       // -stall
    const double cd0    = 0.01;      // profile drag @ Cl≈0
    const double k      = 0.060;      // induced-like term
    const double cm_c4  = -0.01*.0;      // pitching moment about c/4

    static double deg2rad(const double deg) { return deg * M_PI / 180.0; }
};

namespace detail {
inline double clamp(double x, double lo, double hi) { return std::max(lo, std::min(hi, x)); }
inline double sgn(double x) { return (x >= 0.0) ? 1.0 : -1.0; }
inline double wrapPi(double a) { // map to (-pi, pi]
    constexpr double PI = 3.14159265358979323846;
    constexpr double TWO_PI = 2.0 * PI;
    a = std::fmod(a, TWO_PI);
    if (a <= -PI) a += TWO_PI;
    if (a >   PI) a -= TWO_PI;
    return a;
}
}

//-----------------------------------------------------------

struct ViternaParams {
    // Linear pre-stall
    double a      = 2.0 * M_PI;   // lift curve slope
    double alpha0 = -0.038;       // zero-lift (rad)
    double Cl_max = 1.40;         // +stall cap
    double Cl_min = -1.20;        // -stall cap
    double alpha_s_pos = 15.0 * M_PI / 180.0; // +stall angle
    // Parabolic drag in pre-stall
    double Cd0 = 0.008;
    double k   = 0.010;
    // Post-stall / 90°
    double Cd90 = 1.98;           // 2-D plate at 90°
    // Pitching moment about c/4 (pre-stall)
    double Cm0  = -0.05;
};

class ViternaAirfoil2412 final : public IAirfoilPolars {
public:

    explicit ViternaAirfoil2412(const ViternaParams& p = ViternaParams()) : P(p) {
        // Precompute stall match at +alpha_s for Viterna
        const double as = P.alpha_s_pos;
        const double Cls = detail::clamp(P.a * (as - P.alpha0), P.Cl_min, P.Cl_max);
        const double Cds = P.Cd0 + P.k * Cls * Cls;

        // Viterna coefficients
        // B = (Cd_s - Cd90*sin^2(as))/cos(as)
        B_ = (Cds - P.Cd90 * std::sin(as) * std::sin(as)) / std::cos(as);
        // A = (Cl_s - B*sin^2(as)) / sin(2as)
        A_ = (Cls - B_ * std::sin(as) * std::sin(as)) / std::sin(2.0 * as);

        invWidth_ = 1.0 / (M_PI_2 - P.alpha_s_pos); // for Cm blending
    }

    Math::FPointSet GetProfileVertices(int N, float chord_length, float thickness_in_units_of_chord) const override;

    // --- IAirfoil ---
    double Cl(double AlphaRad) const override {
        const double a = detail::wrapPi(AlphaRad);
        const double s = detail::sgn(a);

        if (const double beta = std::fabs(a); beta <= M_PI_2) {
            if (beta <= P.alpha_s_pos) {
                double cl = P.a * (s * beta - P.alpha0);
                cl = detail::clamp(cl, P.Cl_min, P.Cl_max);
                return cl;
            }

            // Viterna from stall to 90°
            const double cl_mag = A_ * std::sin(2.0 * beta) + B_ * std::sin(beta) * std::sin(beta);
            return s * cl_mag;
        } else {
            // 90°..180°: mirror to (pi - beta) and flip lift sign
            const double beta2 = M_PI - beta; // in [0, pi/2)
            const double cl_pos = beta2 <= P.alpha_s_pos
                ? detail::clamp(P.a * (beta2 - P.alpha0), P.Cl_min, P.Cl_max)
                : A_ * std::sin(2.0 * beta2) + B_ * std::sin(beta2) * std::sin(beta2);
            return -s * cl_pos;
        }
    }

    double Cd(const double AlphaRad) const override {
        const double a = detail::wrapPi(AlphaRad);

        if (const double beta = std::fabs(a); beta <= M_PI_2) {
            if (beta <= P.alpha_s_pos) {
                const double cl = Cl(a); // already linear and capped
                return P.Cd0 + P.k * cl * cl;
            }

            return P.Cd90 * std::sin(beta) * std::sin(beta) + B_ * std::cos(beta);
        } else {
            // 90°..180°: Cd(α) = Cd(π − |α|)
            if (const double beta2 = M_PI - beta; beta2 <= P.alpha_s_pos) {
                // Evaluate Cd at +beta2 pre-stall
                const double cl = detail::clamp(P.a * (beta2 - P.alpha0), P.Cl_min, P.Cl_max);
                return P.Cd0 + P.k * cl * cl;
            } else {
                return P.Cd90 * std::sin(beta2) * std::sin(beta2) + B_ * std::cos(beta2);
            }
        }
    }

    double Cm_c4(const double AlphaRad) const override {
        const double a = detail::wrapPi(AlphaRad);
        const double s = detail::sgn(a);

        if (const double beta = std::fabs(a); beta <= M_PI_2) {
            if (beta <= P.alpha_s_pos) {
                return s * P.Cm0; // odd symmetry pre-stall
            }

            // Blend to zero by 90°
            const double w = 1.0 - (beta - P.alpha_s_pos) * invWidth_;
            return s * P.Cm0 * detail::clamp(w, 0.0, 1.0);
        }

        return 0.0; // 90°..180°
    }

    Str GetName() const override { return "NACA2412-Viterna"; };

private:
    ViternaParams P;
    double A_{0.0}, B_{0.0};
    double invWidth_{1.0};
};

} // namespace Foil

#endif //STUDIOSLAB_NACA2412_H