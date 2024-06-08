#ifndef LENNARD_JONES_H
#define LENNARD_JONES_H

#include "Hamiltonians/Langevin.h"

namespace MolecularDynamics {

    class LennardJones : public Langevin {
        const Real L;

    protected:
        Real U(const Graphics::Point2D &q1, const Graphics::Point2D &q2) override;

        Graphics::Point2D dUdr(const Graphics::Point2D &q1, const Graphics::Point2D &q2) override;

    public:
        LennardJones(const Math::NumericConfig &);

        static Real U(Real r);
    };

}



#endif // LENNARD_JONES_H
