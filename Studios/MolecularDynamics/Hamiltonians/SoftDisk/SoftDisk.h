#ifndef SOFT_DISK_H
#define SOFT_DISK_H

#include "Hamiltonians/Langevin.h"
#include "Common/Types.h"

namespace MolecularDynamics {

    class SoftDisk : public Langevin {
    public:
        SoftDisk(const NumericParams &p, Real T);

    protected:
        Real U(const Point2D &q1, const Point2D &q2) override;

        Point2D dUdr(const Point2D &q1, const Point2D &q2) override;
    };

}

#endif // SOFT_DISK_H
