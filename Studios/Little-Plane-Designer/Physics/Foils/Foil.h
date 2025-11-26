//
// Created by joao on 10/16/25.
//

#ifndef STUDIOSLAB_FOIL_H
#define STUDIOSLAB_FOIL_H

#include "../../Render/DebugDraw.h"
#include "box2d/box2d.h"
#include "Math/Geometry/Polygon.h"
#include "Math/VectorSpace/Impl/PointSet.h"
#include "Utils/RandUtils.h"
#include "Utils/String.h"

using namespace Slab;

namespace Foil
{

struct FAirfoilParams {
    const Str Name = ToStr(RandUtils::RandomUniformUInt());

    const float ChordLength = 1.0f;
    const float ThicknessInUnitsOfChortLength  = 0.1f; // units of chord
    const float Span  = 2.0f;

    // Local revolute joint anchor, relative to body local origin
    const Math::Real2D LocalAnchor = {.0, .0};
};

class IAirfoilPolars
{
    public:
    virtual ~IAirfoilPolars() = default;

    virtual Str GetName() const = 0;

    virtual Math::Geometry::FPolygon GetProfileVertices(int N, float chord_length, float thickness_in_units_of_chord) const = 0;

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
    b2Vec2 drag, lift, loc;
    float torque;
    float Cl, Cd, Cm_c4;
    float AoA;
    b2Vec2 GetTotalForce() const { return drag+lift; }
    static auto Null() { return FAirfoilDynamicData{b2Vec2(0.0f, 0.0f), b2Vec2(0.0f, 0.0f), b2Vec2(0.0f, 0.0f), 0.0f}; }
};

inline Graphics::Point2D ToPoint2D(const b2Vec2& v) { return Graphics::Point2D{v.x, v.y}; }

}


#endif //STUDIOSLAB_FOIL_H