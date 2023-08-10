//
// Created by joao on 05/08/2021.
//

#ifndef MOLEKUL_PLAY_NEWTONMECHANICS_H
#define MOLEKUL_PLAY_NEWTONMECHANICS_H

#include "Hamiltonians/Hash/MoleculeSpaceHash.h"

#include "Common/Types.h"
#include "Phys/Numerics/Program/NumericParams.h"


namespace MolecularDynamics {

    class NewtonMechanics {
        MoleculeSpaceHash spaceHash;
    protected:
        const NumericParams &params;

        virtual void applyBoundaryConditions(PointContainer &v_q);

        virtual Real U(const Point2D &q1, const Point2D &q2) { return 0; };

        virtual Point2D mdUdr(const Point2D &q1, const Point2D &q2) = 0;

    public:
        NewtonMechanics(const NumericParams &);

        ~NewtonMechanics();

        Real computeEnergy(const PointContainer &v_q, PointContainer &v_p);

        void operator()(const PointContainer &q,
                        const PointContainer &p,
                        PointContainer &dpdt, double /* t */);


        // TODO: delete these bools
        bool *flippedSides;
        Real dissipation;

    };

}

#endif //MOLEKUL_PLAY_NEWTONMECHANICS_H
