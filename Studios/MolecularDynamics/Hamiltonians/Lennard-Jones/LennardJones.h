#ifndef LENNARD_JONES_H
#define LENNARD_JONES_H

#include "Hamiltonians/Langevin.h"

namespace MolecularDynamics {

    class LennardJones : public Langevin {
        const Real L;

    protected:
        Real U(const Point2D &q1, const Point2D &q2) override;

        Point2D dUdr(const Point2D &q1, const Point2D &q2) override;

    public:
        LennardJones(const NumericConfig &);

        static Real U(Real r);
    };

}



#endif // LENNARD_JONES_H
