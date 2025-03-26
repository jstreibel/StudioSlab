//
// Created by joao on 25/03/25.
//

#ifndef SPI_SOLVER_H
#define SPI_SOLVER_H

#include <Math/Function/R2toR/Model/R2toRNumericFunction.h>
#include <Math/Function/RtoR/Model/RtoRFunction.h>
#include <Math/Numerics/ODE/Solver/LinearStepSolver.h>
#include <Math/VectorSpace/Algebra/Operator.h>

namespace Slab::Models::StochasticPathIntegrals {
    class SPIBC;

    class SPISolver : public Math::Base::LinearStepSolver {
        using SPINumericFunction = Math::Base::NumericFunction<Math::Real2D, Real>;

        Pointer<Math::R2toR::NumericFunction> langevinImpulses = nullptr;
        void ComputeImpulses(Real dτ) const;

        Pointer<Math::Operator<SPINumericFunction>> O;
        Pointer<SPINumericFunction> temp1, temp2;
        Pointer<Math::RtoR::Function> dVdϕ_ptr;

    public:
        explicit SPISolver(const Pointer<SPIBC>&);
        ~SPISolver() override;

        Math::Base::EquationState &
        F(const Math::Base::EquationState &in, Math::Base::EquationState &out, Real t) override;

        void startStep(const Math::Base::EquationState &in, Real t, Real dt) override;
    };

}

#endif //SPI_SOLVER_H
