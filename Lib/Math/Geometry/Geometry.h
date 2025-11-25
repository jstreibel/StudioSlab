//
// Created by joao on 11/22/25.
//

#ifndef STUDIOSLAB_GEOMETRY_H
#define STUDIOSLAB_GEOMETRY_H
#include "Math/Point.h"
#include "Math/VectorSpace/Impl/PointSet.h"
#include "Utils/Result.h"

namespace Slab::Math::Geometry {

TResult<Point2D> ComputeCentroid(const FPointSet& NonIntersecting, bool ForceNoValidation = true);

Real64 ComputeSignedArea(const FPointSet& NonIntersecting);

Real64 ComputeArea(const FPointSet& NonIntersecting);

bool IsValid(const FPointSet& PerhapsIntersecting);

struct RectR {
    Real64 left, right, bottom, top;

    Real64 Width() const { return right - left; }
    Real64 Height() const { return top - bottom; }
};

struct IGeometricObject {
    IGeometricObject() = default;
    virtual ~IGeometricObject() = default;

    virtual FPointSet GetPoints() const = 0;

    // virtual auto GetArea() const -> Real64 = 0;
    // virtual auto GetCentroid() const -> Point2D = 0;
    // virtual auto IsValid() const -> bool = 0;
    // virtual auto GetBoundingBox() const -> RectR;
};

struct FCircle final : IGeometricObject {
    Point2D Center;
    Real64 Radius;

    Real64 BeginAngle = 0;
    Real64 EndAngle = 2 * M_PI;

    FCircle(const Point2D& Center, const Real64 Radius, const Real64 BeginAngle=0., const Real64 EndAngle=2*M_PI)
    : Center(Center)
    , Radius(Radius)
    , BeginAngle(BeginAngle)
    , EndAngle(EndAngle) {    }

    FPointSet GetPoints() const override;
};

struct FAABox final : IGeometricObject {
    Point2D Min, Max;
    FAABox(const Point2D& Min, const Point2D& Max) : Min(Min), Max(Max) {}

    Real2D GetSize() const { return Max - Min; }
    Real64 GetWidth() const { return Max.x - Min.x; }
    Real64 GetHeight() const { return Max.y - Min.y; }
    Point2D GetCenter() const { return (Min + Max) / 2; }
    Real64 GetArea() const { return GetWidth() * GetHeight(); }

    FPointSet GetPoints() const override;
};

}

#endif //STUDIOSLAB_GEOMETRY_H