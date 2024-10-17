/*
 * stepper-rk.h
 *
 *  Created on: 16 de set de 2017
 *      Author: johnny
 */

#ifndef STEPPER_RK_H
#define STEPPER_RK_H

#include "Utils/CyclicIterator.h"

#include "Stepper.h"

#include "Math/Numerics/Solver/BoundaryConditions.h"
#include "Math/Numerics/Solver/Solver.h"
#include "Math/Numerics/NumericalRecipe.h"
#include "Utils/Threads.h"

#include <cstring> // contains memcpy
#include <omp.h>


namespace Slab::Math {

    class RungeKutta4 : public Stepper {
        Mutex mutey;

        Base::Solver_ptr _H;

        Base::EquationState_ptr _f, _k1, _k2, _k3, _k4;
        Base::EquationState_ptr _temp;

        Vector<Base::EquationState_ptr> swaps;
        CyclicIterator<Base::EquationState_ptr> swapsIterator;

        Count steps = 0;

        Real dt;
    public:

        explicit RungeKutta4(const Pointer<Base::Solver> &solver, Real dt, Count totalSwapStates = 5);

        ~RungeKutta4() override = default;

        void step(const Count n_steps) override;

        [[nodiscard]] auto getCurrentState() const -> Base::EquationState_constptr override;

    };


}


#endif /* STEPPER_NR_RK_H_ */
