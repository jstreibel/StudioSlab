
#include "LennardJones.h"
#include "IO/UserInput.h"
#include "LennardJonesParams.h"
#include "Hamiltonians/NewtonMechanicsParams.h"

#include <cmath>

#define POW6(A) (A*A*A*A*A*A)
#define POW7(A) (POW6(A)*A)
#define POW12(A) (POW6(A)*POW6(A))
#define POW13(A) (POW12(A)*A)


inline Point2D LennardJones::mdUdr(const Point2D &q1, const Point2D &q2) {
    Real sqrCutoffRadius = CUTOFF_RADIUS * CUTOFF_RADIUS;
    Real distSqr;

    const Point2D points[] = {
            q2 - q1,
            Point2D(q2.x - SPACE_L, q2.y) - q1,
            Point2D(q2.x + SPACE_L, q2.y) - q1,
            Point2D(q2.x, q2.y - SPACE_L) - q1,
            Point2D(q2.x, q2.y + SPACE_L) - q1,

            Point2D(q2.x - SPACE_L, q2.y - SPACE_L) - q1,
            Point2D(q2.x - SPACE_L, q2.y + SPACE_L) - q1,
            Point2D(q2.x + SPACE_L, q2.y - SPACE_L) - q1,
            Point2D(q2.x + SPACE_L, q2.y + SPACE_L) - q1
    };

    Point2D resultForce = {.0, .0};
    for (auto r : points) {
        distSqr = r.lengthSqr();

        if (distSqr < 4 * sqrCutoffRadius) {
            const auto r_abs = sqrt(distSqr);
            const auto inv_n = σ / r_abs;

            const auto F = 24 * ε / σ * (2 * POW13(inv_n) - POW7(inv_n));

            const Point2D force = -F/r_abs  * r;

            resultForce = resultForce + force;
        }
    }

    return resultForce;
}

Real LennardJones::U(Real r) {
    auto inv_n = σ / r;
    return 4. * ε * (POW12(inv_n) - POW6(inv_n));
}

Real LennardJones::U(const Point2D &q1, const Point2D &q2) {
    const Real SIGMA_SQR = σ * σ;
    Real distSqr;

    const Point2D points[] = {
            q2 - q1,

            Point2D(q2.x - SPACE_L, q2.y) - q1,
            Point2D(q2.x + SPACE_L, q2.y) - q1,
            Point2D(q2.x, q2.y - SPACE_L) - q1,
            Point2D(q2.x, q2.y + SPACE_L) - q1,

            Point2D(q2.x - SPACE_L, q2.y - SPACE_L) - q1,
            Point2D(q2.x - SPACE_L, q2.y + SPACE_L) - q1,
            Point2D(q2.x + SPACE_L, q2.y - SPACE_L) - q1,
            Point2D(q2.x + SPACE_L, q2.y + SPACE_L) - q1
    };

    for (auto r : points) {
        distSqr = r.lengthSqr();
        if (distSqr < SIGMA_SQR) {
            const Real dist = sqrt(distSqr);
            const DoubleAccess arg = {1 - dist / σ};

            return ε / 1. * pow(arg.val, 1.) * arg.isPositive();
        }
    }

    return 0.0;
}