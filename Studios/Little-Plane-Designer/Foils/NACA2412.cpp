//
// Created by joao on 10/20/25.
//

#include "NACA2412.h"

static Math::PointSet GetNACA2412ProfileVertices(int N, float chord_length, float thickness){
    if (N < 2) N = 2;
    Math::Point2DVec v;
    v.reserve(2 * (N + 1));

    // NACA m=camber, p=camber pos, t=thickness (unit chord)
    constexpr double m = 0.02;     // 2412
    constexpr double p = 0.40;
    const double t = thickness/chord_length;

    // ---- Geometry helpers ----
    // NACA 4-digit formulas on x∈[0,1]
    auto yt = [t = thickness](const double x) {
        const double rt = std::sqrt(std::max(0.0, x));
        return 5.0 * t * (0.2969 * rt - 0.1260 * x - 0.3516 * x * x
                          + 0.2843 * x * x * x - 0.1015 * x * x * x * x);
    };
    auto yc = [](const double x) {
        if (x < p) return (m / (p * p)) * (2.0 * p * x - x * x);
        return (m / ((1.0 - p) * (1.0 - p))) * ((1.0 - 2.0 * p) + 2.0 * p * x - x * x);
    };
    auto dyc_dx = [](const double x) {
        if (x < p) return 2.0 * m / (p * p) * (p - x);
        return  2.0 * m / ((1.0 - p) * (1.0 - p)) * (p - x);
    };

    // Lower surface from TE->LE
    for (int i = 0; i <= N; ++i) {
        const double x = 1.0 - static_cast<double>(i) / N; // 1..0
        const double th = std::atan(dyc_dx(x));
        const double yt_ = yt(x);
        const double yc_ = yc(x);
        double xl = x + yt_ * std::sin(th);
        double yl = yc_ - yt_ * std::cos(th);
        v.push_back({xl*chord_length, yl});
    }
    // Upper surface from LE->TE
    for (int i = 0; i <= N; ++i) {
        const double x = static_cast<double>(i) / N; // 0..1
        const double th = std::atan(dyc_dx(x));
        const double yt_ = yt(x);
        const double yc_ = yc(x);
        double xu = x - yt_ * std::sin(th);
        double yu = yc_ + yt_ * std::cos(th);
        v.push_back({xu*chord_length, yu});
    }
    return Math::PointSet(v);
}

Math::PointSet Foil::Airfoil_NACA2412::GetProfileVertices(int N, float chord_length, float thickness) const {
    return GetNACA2412ProfileVertices(N, chord_length, thickness);
}

Math::PointSet Foil::ViternaAirfoil2412::GetProfileVertices(int N, float chord_length, float thickness) const {
    return GetNACA2412ProfileVertices(N, chord_length, thickness);
}