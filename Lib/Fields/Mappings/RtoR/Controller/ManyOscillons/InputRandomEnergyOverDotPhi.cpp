//
// Created by joao on 7/25/22.
//

#include "InputRandomEnergyOverDotPhi.h"
#include "Phys/Numerics/Allocator.h"
#include "Fields/Mappings/RtoR/Model/RtoRFunctionArbitraryCPU.h"
#include "Fields/Mappings/RtoR/Model/FunctionsCollection/NullFunction.h"
#include "Phys/Thermal/Utils/RandUtils.h"
#include "../../Model/RtoRBoundaryCondition.h"

RtoR::InputRandomEnergyOverDotPhi::InputRandomEnergyOverDotPhi()
: RtoRBCInterface("Energy density over time-derivative of field.")
{
    addParameters({&E});
}

auto RtoR::InputRandomEnergyOverDotPhi::getBoundary() const -> const void* {
    auto &allocator = Allocator::getInstance();

    auto N = allocator.getNumericParams().getN();
    auto h = allocator.getNumericParams().geth();
    auto dotPhi = (RtoR::FunctionArbitraryCPU*)allocator.newFunctionArbitrary();

    VecFloat p(N);

    double E_rand=.0;
    for(int i=0; i<N; ++i) {
        auto p_i = RandUtils::random01() - .5;

        p[i] = p_i;
        E_rand += p_i*p_i;
    }

    E_rand*=.5*h;

    auto _E = *E/.75; // dividido por .75 porque o sistema acaba perdendo 25% de energia na termalizacao.
    auto r = sqrt(_E/E_rand);
    for(auto &p_i : p) p_i *= r;

    E_rand = .0;
    for(auto &p_i : p) E_rand += p_i * p_i;
    E_rand *= .5*h;

    auto phi = NullFunction();
    dotPhi->Set(p);

    // TODO: consertar vazamento de memoria com instanciacao do dotPhi;

    return new BoundaryCondition(NullFunction().Clone(), dotPhi->Clone());
}

