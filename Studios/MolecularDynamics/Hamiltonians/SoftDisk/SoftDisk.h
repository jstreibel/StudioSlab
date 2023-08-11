#ifndef SOFT_DISK_H
#define SOFT_DISK_H

#include "Hamiltonians/Langevin.h"
#include "Common/Types.h"

namespace MolecularDynamics {

    class SoftDisk : public Langevin {
    protected:
        Real U(const Point2D &q1, const Point2D &q2) override;

        Point2D dUdr(const Point2D &q1, const Point2D &q2) override;

    public:
        SoftDisk(const NumericParams &p, Real T);

        static Real U(Real r);
    };

}

#endif // SOFT_DISK_H
