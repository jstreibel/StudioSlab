//
// Created by joao on 11/22/25.
//

#include "Geometry.h"

#include <vector>
#include <cmath>
#include <string>
#include <iostream>
#include <catch2/internal/catch_clara.hpp>

#include "imgui_node_editor.h"


namespace Slab::Math::Geometry {

// ----- basic helpers -----

int sign(double v, double eps = EPS) {
    if (v > eps) return 1;
    if (v < -eps) return -1;
    return 0;
}

bool points_equal(const Point2D& a, const Point2D& b, double eps = EPS) {
    return std::fabs(a.x - b.x) <= eps && std::fabs(a.y - b.y) <= eps;
}

// orientation of triplet (a,b,c): >0 CCW, <0 CW, =0 collinear
int orient(const Point2D& a, const Point2D& b, const Point2D& c) {
    const double val = (b.x - a.x) * (c.y - a.y) -
                 (b.y - a.y) * (c.x - a.x);
    return sign(val);
}

bool on_segment(const Point2D& a, const Point2D& b, const Point2D& p) {
    // assumes collinear
    return (p.x >= std::min(a.x, b.x) - EPS &&
            p.x <= std::max(a.x, b.x) + EPS &&
            p.y >= std::min(a.y, b.y) - EPS &&
            p.y <= std::max(a.y, b.y) + EPS);
}

bool segments_intersect(const Point2D& p1, const Point2D& p2,
                        const Point2D& q1, const Point2D& q2)
{
    const int o1 = orient(p1, p2, q1);
    const int o2 = orient(p1, p2, q2);
    const int o3 = orient(q1, q2, p1);
    const int o4 = orient(q1, q2, p2);

    // Proper intersection
    if (o1 * o2 < 0 && o3 * o4 < 0)
        return true;

    // Collinear & overlapping cases
    if (o1 == 0 && on_segment(p1, p2, q1)) return true;
    if (o2 == 0 && on_segment(p1, p2, q2)) return true;
    if (o3 == 0 && on_segment(q1, q2, p1)) return true;
    if (o4 == 0 && on_segment(q1, q2, p2)) return true;

    return false;
}

// ----- polygon helpers -----
// Input is an open ring: pts[0..n-1], implicitly closed with edge (n-1 -> 0)

double polygon_signed_area(const std::vector<Point2D>& pts) {
    const int n = static_cast<int>(pts.size());
    if (n < 3) return 0.0;

    double a = 0.0;
    for (int i = 0; i < n; ++i) {
        const Point2D& p = pts[i];
        const Point2D& q = pts[(i + 1) % n];
        a += p.x * q.y - q.x * p.y;
    }
    return 0.5 * a;
}

// detects any self-intersection among edges, excluding shared endpoints
bool polygon_self_intersects(const std::vector<Point2D>& pts) {
    const int n = static_cast<int>(pts.size());
    if (n < 3) return false;

    auto edge = [&](int i) {
        const Point2D& a = pts[i];
        const Point2D& b = pts[(i + 1) % n];
        return std::pair<Point2D, Point2D>(a, b);
    };

    for (int i = 0; i < n; ++i) {
        auto [p1, p2] = edge(i);
        for (int j = i + 1; j < n; ++j) {
            // Skip same edge or adjacent edges (sharing a vertex)
            if (j == i) continue;
            if (j == (i + 1) % n) continue;
            if (i == (j + 1) % n) continue;

            auto [q1, q2] = edge(j);

            if (segments_intersect(p1, p2, q1, q2)) {
                return true;
            }
        }
    }
    return false;
}

// sanitize input: remove duplicated closing vertex if present
std::vector<Point2D> normalize_open_ring(const std::vector<Point2D>& pts) {
    if (pts.size() >= 2 && points_equal(pts.front(), pts.back())) {
        return std::vector(pts.begin(), pts.end() - 1);
    }
    return pts; // already open
}

// ----- public API -----

FPolygonValidationResult validate_polygon(const std::vector<Point2D>& raw_pts) {
    std::vector<Point2D> pts = normalize_open_ring(raw_pts);
    const int n = static_cast<int>(pts.size());

    if (n < 3) return {FPolygonValidationResult::TooFewPoints};

    // Check for consecutive duplicate vertices (zero-length edges)
    for (int i = 0; i < n; ++i) {
        const Point2D& a = pts[i];
        const Point2D& b = pts[(i + 1) % n];
        if (points_equal(a, b))
            return {FPolygonValidationResult::ZeroLengthEdge};
    }

    // Self-intersections
    if (polygon_self_intersects(pts))
        return {FPolygonValidationResult::SelfIntersecting};

    // Nonzero area
    double A = polygon_signed_area(pts);
    if (std::fabs(A) <= EPS)
        return {FPolygonValidationResult::Degenerate};

    return {FPolygonValidationResult::Ok};
}

// Compute centroid (center of mass for constant density).
// Returns true on success; false if the polygon is degenerate (area ~ 0).
bool polygon_centroid(const std::vector<Point2D>& raw_pts, double& cx, double& cy) {
    const std::vector<Point2D> pts = normalize_open_ring(raw_pts);
    const int n = static_cast<int>(pts.size());
    if (n < 3) return false;

    double A2 = 0.0;   // 2 * area
    double Cx = 0.0;
    double Cy = 0.0;

    for (int i = 0; i < n; ++i) {
        const Point2D& p = pts[i];
        const Point2D& q = pts[(i + 1) % n];
        double cross = p.x * q.y - q.x * p.y;
        A2 += cross;
        Cx += (p.x + q.x) * cross;
        Cy += (p.y + q.y) * cross;
    }

    if (std::fabs(A2) <= EPS) {
        return false; // degenerate
    }

    cx = Cx / (3.0 * A2);  // equivalently Cx / (6*A)
    cy = Cy / (3.0 * A2);
    return true;
}

TResult<Point2D> ComputeCentroid(const FPointSet& NonIntersecting, const bool ForceNoValidation) {
    if (!ForceNoValidation && !ValidatePolygon(NonIntersecting)) TResult<Point2D>::Fail("Invalid geometry: either intersecting or degenerate.");

    double cx, cy;
    if (!polygon_centroid(NonIntersecting.GetPoints(), cx, cy)) {
        return TResult<Point2D>::Fail("Degenerate geometry");
    }

    return TResult<Point2D>::Ok(Point2D{cx, cy});
}

Real64 ComputeSignedArea(const FPointSet& NonIntersecting) {
    return polygon_signed_area(NonIntersecting.GetPoints());
}


Real64 ComputeArea(const FPointSet& NonIntersecting) {
    return fabs(ComputeSignedArea(NonIntersecting));
}

FPolygonValidationResult ValidatePolygon(const FPointSet& PerhapsIntersecting) {
    return validate_polygon(PerhapsIntersecting.GetPoints());
}

FPointSet FCircle::GetPoints() const {
    constexpr int MaxN = 100;

    std::vector<Point2D> Points;
    Points.reserve(MaxN);

    constexpr Real64 da = 2*M_PI / MaxN;
    for (Real64 Angle=BeginAngle; Angle <= EndAngle+EPS; Angle+=da) {
        Points.emplace_back(Center.x + Radius * std::cos(Angle), Center.y + Radius * std::sin(Angle));
    }

    return FPointSet(Points);
}

FPointSet FAABox::GetPoints() const {
    FPointSet Points;
    Points.AddPoint(Min);
    fix dx = Real2D{GetWidth(), 0};
    return FPointSet({Min, Min+dx, Max, Max-dx});
}
}