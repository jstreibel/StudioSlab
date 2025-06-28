
#include "LennardJones.h"
#include "LennardJonesParams.h"

#include <cmath>

#define POW6(A) (A*A*A*A*A*A)
#define POW7(A) (POW6(A)*A)
#define POW12(A) (POW6(A)*POW6(A))
#define POW13(A) (POW12(A)*A)

namespace Slab::Models::MolecularDynamics {

    LennardJones::LennardJones(Pointer<Config> config, DevFloat T)
            : Langevin(config, T) {}

    inline Graphics::Point2D LennardJones::dUdr(const Graphics::Point2D &q1, const Graphics::Point2D &q2) {
        DevFloat sqrCutoffRadius = CUTOFF_RADIUS * CUTOFF_RADIUS;
        DevFloat distSqr;

        fix L = numeric_config->getL();

        const Graphics::Point2D points[] = {
                q2 - q1,
                Graphics::Point2D(q2.x - L, q2.y) - q1,
                Graphics::Point2D(q2.x + L, q2.y) - q1,
                Graphics::Point2D(q2.x, q2.y - L) - q1,
                Graphics::Point2D(q2.x, q2.y + L) - q1,

                Graphics::Point2D(q2.x - L, q2.y - L) - q1,
                Graphics::Point2D(q2.x - L, q2.y + L) - q1,
                Graphics::Point2D(q2.x + L, q2.y - L) - q1,
                Graphics::Point2D(q2.x + L, q2.y + L) - q1
        };

        Graphics::Point2D resultForce = {.0, .0};
        for (auto r: points) {
            distSqr = r.lengthSqr();

            if (distSqr < 4 * sqrCutoffRadius) {
                const auto r_abs = sqrt(distSqr);
                const auto inv_n = σ / r_abs;

                const auto F = 24 * ε / σ * (2 * POW13(inv_n) - POW7(inv_n));

                const Graphics::Point2D force = -F / r_abs * r;

                resultForce = resultForce + force;
            }
        }

        return resultForce;
    }

    DevFloat MolecularDynamics::LennardJones::U(DevFloat r) {
        auto inv_n = σ / r;
        return 4. * ε * (POW12(inv_n) - POW6(inv_n));
    }

    DevFloat MolecularDynamics::LennardJones::U(const Graphics::Point2D &q1, const Graphics::Point2D &q2) {
        const DevFloat SIGMA_SQR = σ * σ;
        DevFloat distSqr;

        fix L = numeric_config->getL();

        const Graphics::Point2D points[] = {
                q2 - q1,

                Graphics::Point2D(q2.x - L, q2.y) - q1,
                Graphics::Point2D(q2.x + L, q2.y) - q1,
                Graphics::Point2D(q2.x, q2.y - L) - q1,
                Graphics::Point2D(q2.x, q2.y + L) - q1,

                Graphics::Point2D(q2.x - L, q2.y - L) - q1,
                Graphics::Point2D(q2.x - L, q2.y + L) - q1,
                Graphics::Point2D(q2.x + L, q2.y - L) - q1,
                Graphics::Point2D(q2.x + L, q2.y + L) - q1
        };

        for (auto r: points) {
            distSqr = r.lengthSqr();
            if (distSqr < SIGMA_SQR) {
                const DevFloat dist = sqrt(distSqr);
                const DoubleAccess arg = {1 - dist / σ};

                return ε / 1. * pow(arg.val, 1.) * arg.isPositive();
            }
        }

        return 0.0;
    }


}