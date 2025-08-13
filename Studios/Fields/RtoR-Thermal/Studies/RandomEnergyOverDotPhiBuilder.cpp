//
// Created by joao on 7/25/22.
//

#include "RandomEnergyOverDotPhiBuilder.h"
#include "Math/Function/RtoR/Model/RtoRNumericFunctionCPU.h"
#include "Math/Function/RtoR/Model/FunctionsCollection/NullFunction.h"
#include "Utils/RandUtils.h"
#include "Models/KleinGordon/RtoR/LinearStepping/KG-RtoRBoundaryCondition.h"


namespace Studios::Fields::RtoRThermal {

    using namespace Slab;
    using namespace Slab::Math;

    RandomEnergyOverDotPhiBuilder::RandomEnergyOverDotPhiBuilder()
            : Builder("Random dphipt", "Energy density over time-derivative of field") {
        Interface->AddParameters({&E});
    }

    auto RandomEnergyOverDotPhiBuilder::GetBoundary() -> Base::BoundaryConditions_ptr {
        auto N = kg_numeric_config->getN();
        auto h = kg_numeric_config->geth();
        auto dotPhi = newFunctionArbitrary();

        auto &p = dotPhi->getSpace().getHostData(false);

        auto proto = NewFieldState();

        if (1) {
            DevFloat E_rand = .0;
            for (int i = 0; i < N; ++i) {
                auto p_i = RandUtils::RandomUniformReal01() - .5;

                p[i] = p_i;
                E_rand += p_i * p_i;
            }

            E_rand *= .5 * h;

            auto _E =
                    *E / .75; // dividido por .75 porque o sistema acaba perdendo 25% de energia na termalizacao.
            auto r = sqrt(_E / E_rand);
            for (auto &p_i: p) p_i *= r;

            E_rand = .0;
            for (auto &p_i: p) E_rand += p_i * p_i;
            E_rand *= .5 * h;

            auto phi = RtoR::NullFunction();

            return New<Slab::Models::KGRtoR::BoundaryCondition>(proto, RtoR::NullFunction().Clone(), dotPhi);

        } else {
            return New<Slab::Models::KGRtoR::BoundaryCondition>(proto, RtoR::NullFunction().Clone(), RtoR::NullFunction().Clone());
        }
    }


}