
#include "SoftDisk.h"
#include "SoftDiskParams.h"

#include <cmath>

namespace Slab::Models::MolecularDynamics {

    using Config = Slab::Models::MolecularDynamics::MolDynNumericConfig;

    SoftDisk::SoftDisk(TPointer<Config> config, DevFloat T)
    : Langevin(config, T) {

    }

    inline Graphics::Point2D SoftDisk::dUdr(const Graphics::Point2D &q1, const Graphics::Point2D &q2) {
        const DevFloat SIGMA_SQR = σ * σ;
        DevFloat normSqr;

        fix L = numeric_config->GetL();

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
            normSqr = r.LengthSqr();

            if (normSqr < SIGMA_SQR) {
                const DevFloat norm = r.Length();
                const DoubleAccess arg = {1.0 - norm / σ};
                const DevFloat mag = ε / σ * pow(arg.val, ALPHA - 1.0) * arg.isPositive();

                const Graphics::Point2D force = -(mag / norm) * r;

                resultForce = resultForce + force;
            }
        }

        return resultForce;
    }

    DevFloat SoftDisk::U(const Graphics::Point2D &q1, const Graphics::Point2D &q2) {
        const DevFloat SIGMA_SQR = σ * σ;
        DevFloat normSqr;

        const Graphics::Point2D points[] = {
                q2 - q1,
                /*
                Point2D(q2.x-SIMSPACE_SIDE_SIZE, q2.y)-q1,
                Point2D(q2.x+SIMSPACE_SIDE_SIZE, q2.y)-q1,
                Point2D(q2.x, q2.y-SIMSPACE_SIDE_SIZE)-q1,
                Point2D(q2.x, q2.y+SIMSPACE_SIDE_SIZE)-q1,

                Point2D(q2.x-SIMSPACE_SIDE_SIZE, q2.y-SIMSPACE_SIDE_SIZE)-q1,
                Point2D(q2.x-SIMSPACE_SIDE_SIZE, q2.y+SIMSPACE_SIDE_SIZE)-q1,
                Point2D(q2.x+SIMSPACE_SIDE_SIZE, q2.y-SIMSPACE_SIDE_SIZE)-q1,
                Point2D(q2.x+SIMSPACE_SIDE_SIZE, q2.y+SIMSPACE_SIDE_SIZE)-q1
                 */
        };

        for (auto r: points) {
            normSqr = r.LengthSqr();
            if (normSqr < SIGMA_SQR) {
                const DevFloat norm = sqrt(normSqr);
                const DoubleAccess arg = {1 - norm / σ};

                return ε / ALPHA * pow(arg.val, ALPHA) * arg.isPositive();
            }
        }

        return 0.0;
    }

    DevFloat SoftDisk::U(const DevFloat r) {

        if( r > σ) return .0;

        const DoubleAccess arg = {1 - r / σ};

        return ε / ALPHA * pow(arg.val, ALPHA) * arg.isPositive();
    }
}