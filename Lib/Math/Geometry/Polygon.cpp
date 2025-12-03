//
// Created by joao on 11/25/25.
//

#include "Polygon.h"

#include "Geometry.h"
#include <algorithm>
#include <cmath>

namespace Slab::Math::Geometry {
FMass2DProperties ComputePolygonMassProperties(const FPointSet& Polygon, Real64 Density) {
    fix &Vertices = Polygon.GetPoints();
    const int Count = (int)Vertices.size();
    assert(Count >= 3);

    double Area2 = 0.0;   // 2 * signed area
    double Cx = 0.0;
    double Cy = 0.0;
    double IzAccum = 0.0; // for ∫(x^2 + y^2) dA (up to constant factor)

    for (int i = 0; i < Count; ++i) {
        const auto& p0 = Vertices[i];
        const auto& p1 = Vertices[(i + 1) % Count];

        const double x0 = p0.x;
        const double y0 = p0.y;
        const double x1 = p1.x;
        const double y1 = p1.y;

        const double Cross = x0 * y1 - x1 * y0; // C_i

        Area2 += Cross;

        Cx += (x0 + x1) * Cross;
        Cy += (y0 + y1) * Cross;

        const double x0sq = x0 * x0;
        const double x1sq = x1 * x1;
        const double y0sq = y0 * y0;
        const double y1sq = y1 * y1;

        // Contribution to ∫(x^2 + y^2) dA (geometric)
        IzAccum += Cross * (x0sq + x0 * x1 + x1sq + y0sq + y0 * y1 + y1sq);
    }

    // Signed area; must be non-zero by assumption.
    const double Sign = (Area2 >= 0.0) ? 1.0 : -1.0;
    const double Area = 0.5 * Sign * Area2;    // positive area
    const double Mass = Density * Area;

    // Centroid (works for either CW or CCW because area2 carries the sign)
    const double Inv3Area2 = 1.0 / (3.0 * Area2);
    const double cx = Cx * Inv3Area2;
    const double cy = Cy * Inv3Area2;

    // Geometric I_z about origin: ∫(x^2 + y^2) dA
    const double Iz_geom = (Sign * IzAccum) / 12.0;  // factor 1/12 and fix orientation
    const double InertiaOrigin = Density * Iz_geom;

    // Parallel axis: I_centroid = I_origin - M * |r_cm|^2
    const double r2 = cx * cx + cy * cy;
    const double InertiaCentroid = InertiaOrigin - Mass * r2;

    return {
        .Mass = static_cast<float>(Mass),
        .Centroid = { static_cast<float>(cx), static_cast<float>(cy) },
        .InertiaOrigin = static_cast<float>(InertiaOrigin),
        .InertiaCentroid = static_cast<float>(InertiaCentroid)
    };
}


FPolygon::FPolygon(const FPointSet& PointSet) : FPointSet(PointSet) {
    ValidateWithException();
}

FPolygon::FPolygon(const FPoint2DVec& Points) : FPointSet(Points) {
    ValidateWithException();
}

void FPolygon::ValidateWithException() const {
    if (const auto Validation = Validate(); !Validation)
        throw Exception(ToStr("Invalid Polygon: %s", Validation.ToString().c_str()));
}

FPolygonValidationResult FPolygon::Validate() const {
    return ValidatePolygon(*this);

}

auto FPolygon::ComputeMassProperties(const Real64 &Density) const -> FMass2DProperties {
    return ComputePolygonMassProperties(*this, Density);
}

auto FPolygon::GetBoundingBox() const -> RectR {
    fix Max = GetMax();
    fix Min = GetMin();
    return RectR{Min.x, Max.x, Min.y, Max.y};
}

bool FPolygon::Contains(const Point2D& Point) const {
    const auto& Vertices = GetPoints();
    const int Count = static_cast<int>(Vertices.size());
    if (Count < 3)
        return false;

    if (!GetBoundingBox().Contains(Point))
        return false;

    bool Inside = false;
    for (int i = 0, j = Count - 1; i < Count; j = i++) {
        const auto& Pi = Vertices[i];
        const auto& Pj = Vertices[j];

        if (const double Cross = (Pi.x - Point.x) * (Pj.y - Point.y) - (Pi.y - Point.y) * (Pj.x - Point.x);
            std::fabs(Cross) <= EPS)
        {
            const double MinX = std::min(Pi.x, Pj.x) - EPS;
            const double MaxX = std::max(Pi.x, Pj.x) + EPS;
            const double MinY = std::min(Pi.y, Pj.y) - EPS;
            const double MaxY = std::max(Pi.y, Pj.y) + EPS;
            if (Point.x >= MinX && Point.x <= MaxX && Point.y >= MinY && Point.y <= MaxY)
                return true; // on edge
        }

        const bool Intersects = ((Pi.y > Point.y) != (Pj.y > Point.y)) &&
                                (Point.x < (Pj.x - Pi.x) * (Point.y - Pi.y) / (Pj.y - Pi.y) + Pi.x);
        if (Intersects)
            Inside = !Inside;
    }

    return Inside;
}

FPolygon FPolygon::MakeBox(const Real64 Length, const Real64 Height, const Real2D Location, const Real64 Rotation) {
    fix hw = Length * .5f;
    fix hh = Height * .5f;

    FPointSet Points;
    Points.AddPoint(Location + Point2D{-hw, -hh});
    Points.AddPoint(Location + Point2D{+hw, -hh});
    Points.AddPoint(Location + Point2D{+hw, +hh});
    Points.AddPoint(Location + Point2D{-hw, +hh});

    return FPolygon{Points.Rotate(Rotation)};
}

}
