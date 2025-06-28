//
// Created by joao on 25/03/25.
//

#include "SPI-Solver.h"

#include <boost/core/ref.hpp>
#include <boost/math/constants/constants.hpp>
#include <cairomm/enums.h>
#include <Math/Function/R2toR/Model/Operators/R2toRLaplacian.h>
#include <Math/Function/RtoR/Model/FunctionsCollection/AbsFunction.h>
#include <Math/Function/RtoR/Model/FunctionsCollection/NullFunction.h>

#include "SPI-BC.h"
#include "SPI-State.h"
#include "Math/Function/R2toR/Model/FunctionsCollection/NullFunction.h"

#include "Utils/OMPUtils.h"
#include "Utils/RandUtils.h"

namespace Slab::Models::StochasticPathIntegrals {

    SPISolver::SPISolver(const Pointer<SPIBC>& du) : LinearStepSolver(du) {
        O = New<Math::R2toR::R2toRLaplacian>();
        dVdϕ_ptr = Math::RtoR::AbsFunction().diff(0);
    }

    SPISolver::~SPISolver() = default;

    inline DevFloat ξ() { return RandUtils::GaussianNoise(.0, 1.0); }

    void SPISolver::ComputeImpulses(DevFloat dτ) const {
        if (langevinImpulses == nullptr) return;

        auto &langevin_space = langevinImpulses->getSpace();
        assert(langevin_space.getMetaData().getNDim() == 2);

        auto &langevin_data = langevin_space.getHostData();

        fix a=sqrt(2/dτ);
        for (auto k=0; k < langevin_data.size(); k++) {
            langevin_data[k] = a*ξ();
        }

        langevin_space.upload();
    }

    void SPISolver::startStep(const Math::Base::EquationState &in, DevFloat t, DevFloat dτ) {
        if (temp1 == nullptr) {
            auto &refState = dynamic_cast<const SPIState &>(in);

            temp1            = refState.cloneData();
            temp2            = refState.cloneData();
            langevinImpulses = refState.cloneData();
            langevinImpulses->Set(Math::R2toR::NullFunction());
        }

        LinearStepSolver::startStep(in, t, dτ);

        ComputeImpulses(dτ);
    }

    Math::Base::EquationState &
    SPISolver::F(const Math::Base::EquationState &in,
                 Math::Base::EquationState &out,
                 DevFloat t) {
        auto &spiStateIn = dynamic_cast<const SPIState &>(in);
        auto &spiStateOut = dynamic_cast<SPIState &>(out);

        const auto &ϕᵢₙ  = *spiStateIn.getPhi().get();
        auto       &ϕₒᵤₜ  = *spiStateOut.getPhi().get();


#pragma omp barrier
        {
            fix &eta = *langevinImpulses;
            // fix &δSₑ = *temp1;
            fix &L = *O;
            GET Lϕ = *temp1;
            GET dVdϕ = *temp2;

            Lϕ = L*ϕᵢₙ;
            ϕᵢₙ .Apply(*dVdϕ_ptr, dVdϕ);

            // ϕ(τ+1) = ϕ(t) + Δτ × ( Δ²ϕ(τ) + V'(ϕ(τ)) - √(2/Δτ)eta )

            ϕₒᵤₜ = Lϕ + dVdϕ;
            ϕₒᵤₜ -= eta ;
        }
#pragma omp barrier

        return out;
    }
}
