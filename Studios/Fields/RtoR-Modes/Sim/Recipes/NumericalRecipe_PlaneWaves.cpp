//
// Created by joao on 8/2/25.
//

#include "NumericalRecipe_PlaneWaves.h"

#include "Core/SlabCore.h"

#include "../BoundaryCondition.h"
#include "../Monitor.h"

#define DONT_REGISTER false

namespace Modes {

    // using namespace Slab::Models;

    FNumericalRecipe_PlaneWaves::FNumericalRecipe_PlaneWaves()
    : KGRtoRBuilder("Plane Waves", "Analytic plane waves initial condition", DONT_REGISTER)
    {
        Interface->AddParameters({&Q, &harmonic});

        if constexpr (true) RegisterCLInterface(Interface);

        this->SetLaplacianPeriodicBC();
    }

    Base::BoundaryConditions_ptr FNumericalRecipe_PlaneWaves::GetBoundary() {
        fix L = DynamicPointerCast<KGNumericConfig>(this->getNumericConfig())->GetL();
        // k=2πn/L
        fix n = static_cast<DevFloat>(*harmonic);
        fix dk = 2*M_PI/L;
        fix k = n*dk;
        Log::Info() << "Using L=" << L << " and harmonic n=" << n << " to compute k=" << k << Log::Flush;
        return New <FPlaneWaveBC> (NewFieldState(), *Q,  k);
    }

    Str FNumericalRecipe_PlaneWaves::SuggestFileName() const {
        const auto SEPARATOR = " ";

        const StrVector Params = {"Q", "harmonic"};

        const auto StrParams = Interface->ToString(Params, SEPARATOR);

        return KGRtoRBuilder::SuggestFileName() + SEPARATOR + StrParams;
    }

} // Modes