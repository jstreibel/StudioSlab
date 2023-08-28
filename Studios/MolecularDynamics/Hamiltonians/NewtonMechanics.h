//
// Created by joao on 05/08/2021.
//

#ifndef MOLEKUL_PLAY_NEWTONMECHANICS_H
#define MOLEKUL_PLAY_NEWTONMECHANICS_H

#include "Particle.h"
#include "Hamiltonians/Hash/MoleculeSpaceHash.h"

#include "Math/Numerics/SimConfig/NumericConfig.h"


namespace MolecularDynamics {

    class NewtonMechanics {
        MoleculeSpaceHash spaceHash;

        bool *flippedSides;
        Real dissipation;

    protected:
        const NumericConfig &params;

        virtual void applyBoundaryConditions(PointContainer &v_q);      // For velocity Verlet stepper
        virtual void applyBoundaryConditions(MoleculeContainer &m);     // For RK4 stepper

        virtual Real U(const Point2D &q1, const Point2D &q2) { return 0; };

        virtual Point2D dUdr(const Point2D &q1, const Point2D &q2) = 0;

        /**
         * Non-homogeneous force term
         * @param t current simulation time
         * @return the non-homogeneous force
         */
        virtual Point2D F_nh(Real t) { return {0,0}; }

    public:
        explicit NewtonMechanics(const NumericConfig &);

        ~NewtonMechanics();

        auto setDissipation(Real k) -> void;

        Real computeEnergy(const PointContainer &v_q, PointContainer &v_p);

        // For velocity Verlet stepper
        void operator()(const PointContainer &q,
                        const PointContainer &p,
                        PointContainer &dpdt, Real /* t */);

        // For RK4 stepper
        void operator()(const MoleculeContainer &m, MoleculeContainer &dmdt, Real /* t */);

        auto getFlippedSidesMap() const -> const bool*;

    };

}

#endif //MOLEKUL_PLAY_NEWTONMECHANICS_H
