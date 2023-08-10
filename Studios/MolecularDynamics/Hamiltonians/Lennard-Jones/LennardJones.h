#ifndef MOLDYN_H
#define MOLDYN_H


#include "Hamiltonians/NewtonMechanics.h"
#include "Phys/Numerics/Program/NumericParams.h"

namespace MolecularDynamics {

    class LennardJones : public NewtonMechanics {
        const Real L;

    protected:
        Real U(const Point2D &q1, const Point2D &q2) override;

        Point2D mdUdr(const Point2D &q1, const Point2D &q2) override;

    public:
        LennardJones(const NumericParams &);

        static Real U(Real r);

    };

}



#endif // MOLDYN_H
