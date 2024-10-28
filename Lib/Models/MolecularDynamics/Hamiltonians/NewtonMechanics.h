//
// Created by joao on 05/08/2021.
//

#ifndef MOLEKUL_PLAY_NEWTONMECHANICS_H
#define MOLEKUL_PLAY_NEWTONMECHANICS_H

#include "Particle.h"
#include "Models/MolecularDynamics/Hamiltonians/Hash/MoleculeSpaceHash.h"

#include "Math/Numerics/SimConfig/NumericConfig.h"
#include "Models/MolecularDynamics/MolDynNumericConfig.h"


namespace Slab::Models::MolecularDynamics {

    using Config = Slab::Models::MolecularDynamics::MolDynNumericConfig;

    class NewtonMechanics {
        MoleculeSpaceHash spaceHash;

        bool *flippedSides;
        Real dissipation;

    protected:
        virtual void applyBoundaryConditions(Graphics::PointContainer &v_q);      // For velocity Verlet stepper
        virtual void applyBoundaryConditions(MoleculeContainer &m);     // For RK4 stepper

        virtual Real U(const Graphics::Point2D &q1, const Graphics::Point2D &q2) { return 0; };

        virtual Graphics::Point2D dUdr(const Graphics::Point2D &q1, const Graphics::Point2D &q2) = 0;

        /**
         * Non-homogeneous force term
         * @param t current simulation time
         * @return the non-homogeneous force
         */
        virtual Graphics::Point2D F_nh(Real t) { return {0,0}; }

        Pointer<Config> numeric_config;

    public:

        explicit NewtonMechanics(Pointer<Config> config);

        ~NewtonMechanics();

        auto setDissipation(Real k) -> void;

        Real computeEnergy(const Graphics::PointContainer &v_q, Graphics::PointContainer &v_p);

        // For velocity Verlet stepper
        void operator()(const Graphics::PointContainer &q,
                        const Graphics::PointContainer &p,
                        Graphics::PointContainer &dpdt, Real /* t */);

        // For RK4 stepper
        void operator()(const MoleculeContainer &m, MoleculeContainer &dmdt, Real /* t */);

        auto getFlippedSidesMap() const -> const bool*;

    };

}

#endif //MOLEKUL_PLAY_NEWTONMECHANICS_H
