//
// Created by joao on 8/08/23.
//

#ifndef STUDIOSLAB_MOLDYN_BUILDER_H
#define STUDIOSLAB_MOLDYN_BUILDER_H

#include "../../Core/Controller/Parameter/BuiltinParameters.h"

#include "Math/Numerics/NumericalRecipe.h"
#include "Math/Numerics/OutputChannel.h"

namespace Slab::Models::MolecularDynamics {

using namespace Slab;
using namespace Slab::Core;

class FMolecularDynamicsRecipe final : public Math::Base::FNumericalRecipe {
    RealParameter Temperature = RealParameter(
        0.0, FParameterDescription{'T', "temperature", "The system temperature in Langevin simulations."});
    RealParameter Dissipation = RealParameter(
        0.0, FParameterDescription{'k', "dissipation_factor", "The system energy dissipation factor."});
    IntegerParameter Model = IntegerParameter(1, FParameterDescription{"model", "The physical model to use:"
                                                                       "\n\t\t0: Lennard-Jones"
                                                                       "\n\t\t1: Soft disk dynamics"});

    // FInterface_ptr MolDynamicsInterface;

public:
    FMolecularDynamicsRecipe();

    auto BuildOutputSockets() -> Vector<TPointer<Math::FOutputChannel>> override;

    auto BuildStepper() -> TPointer<Math::FStepper> override;

    auto NotifyInterfaceSetupIsFinished() -> void override;

};

} // MolecularDynamics

#endif //STUDIOSLAB_MOLDYN_BUILDER_H