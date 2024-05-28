//
// Created by joao on 12/08/23.
//

#include "LangevinKGSolver.h"
#include "Math/Function/RtoR/Model/FunctionsCollection/NullFunction.h"
#include "Utils/RandUtils.h"
#include "Utils/OMPUtils.h"


namespace Slab::Models::KGRtoR {

    Real ξ() {
        return RandUtils::gaussianNoise(.0, 1.0);
    }

    LangevinKGSolver::LangevinKGSolver(const NumericConfig &params,
                                       Base::BoundaryConditions_ptr du,
                                       Pointer<Potential> potential)
    : KGRtoRSolver(params, du, potential){

    }

    void LangevinKGSolver::ComputeImpulses() {
        if (langevinImpulses == nullptr) return;

        auto &space = langevinImpulses->getSpace();
        assert(space.getMetaData().getNDim() == 1);

        auto &X = space.getHostData();

        fix nSplit = 1;
        OMP_PARALLEL_FOR(i, X.size() / nSplit) {
            fix x = ξ();
            fix k = nSplit * i;
            for (auto n = 0; n < nSplit; ++n) {
                auto &x1 = X[k + n];
                x1 = x;
            }
        }

        space.upload();
    }

    void LangevinKGSolver::startStep_KG(const EquationState &kgState, Real t, Real dt) {
        KGRtoRSolver::startStep_KG(kgState, t, dt);

        ComputeImpulses();
    }

    EquationState &
    LangevinKGSolver::dtF_KG(const EquationState &kgStateIn, EquationState &kgStateOut, Real t, Real dt) {
        if (langevinImpulses == nullptr) {
            assert(scaledImpulses == nullptr);

            langevinImpulses = DynamicPointerCast <RtoR::DiscreteFunction> (kgStateIn.getPhi().Clone());
            scaledImpulses   = DynamicPointerCast <RtoR::DiscreteFunction> (kgStateIn.getPhi().Clone());

            scaledImpulses->Set(RtoR::NullFunction());
        }

#pragma omp barrier

        kgStateOut = KGSolver::dtF_KG(kgStateIn, kgStateOut, t, dt);

#pragma omp barrier

        {
            IN pi = kgStateIn.getDPhiDt();
            OUT dissipation = *temp1;
            dissipation.StoreMultiplication(pi, -γ);

            fix h = params.geth();
            fix alpha = sqrt(2 * T * γ / dt);
            scaledImpulses->StoreMultiplication(*langevinImpulses, alpha);

            OUT F = kgStateOut.getDPhiDt();
            OUT F_ext = *temp2;
            F_ext.StoreAddition(*scaledImpulses, dissipation) *= dt;

            F.Add(F_ext);
        }

        return kgStateOut;
    }

    void LangevinKGSolver::setTemperature(Real value) { T = value; }

    void LangevinKGSolver::setDissipationCoefficient(Real value) { γ = value; }



}