//
// Created by joao on 11/25/25.
//

#ifndef STUDIOSLAB_POLYGON_H
#define STUDIOSLAB_POLYGON_H
#include "Geometry.h"
#include "Math/VectorSpace/Impl/PointSet.h"

namespace Slab::Math::Geometry {

struct FMass2DProperties {
    Real64 Mass;             // total mass
    Real2D Centroid;   // COM in same coords as input vertices
    Real64 InertiaOrigin;    // I_z about origin
    Real64 InertiaCentroid;  // I_z about centroid
};

// vertices: polygon vertices in order (CW or CCW), non-self-intersecting
// density: mass per unit area
FMass2DProperties ComputePolygonMassProperties(const FPointSet& Polygon, Real64 Density);

struct IPolygon : IGeometricObject {

};

struct FPolygon final : FPointSet
{
    explicit FPolygon(const FPointSet &PointSet);
    explicit FPolygon(const FPoint2DVec &Points);

    void ValidateWithException() const;
    FPolygonValidationResult Validate() const;
    auto ComputeMassProperties(const Real64 &Density) const -> FMass2DProperties;
    auto GetBoundingBox() const -> RectR;
    bool Contains(const Point2D &Point) const;

    static FPolygon MakeBox(Real64 Length, Real64 Height, Real2D Location={}, Real64 Rotation0=0);
};

}

#endif //STUDIOSLAB_POLYGON_H