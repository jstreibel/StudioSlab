#ifndef SIMPERTURBED_H
#define SIMPERTURBED_H

#include "../Builder.h"


namespace Studios::PureSG {

    using namespace Slab::Core;


    class FInputPerturbations : public FBuilder {
        RealParameter l   = RealParameter{1., FParameterDescription{'l', "Initial condition scale factor."}};
        RealParameter eps = RealParameter{1., FParameterDescription{'a', "Perturbation value. a=1 means no perturbation."}};

    public:
        FInputPerturbations();

        auto GetBoundary() -> Math::Base::BoundaryConditions_ptr override;
    };

    using InputPerturbations [[deprecated("Use FInputPerturbations")]] = FInputPerturbations;
}

#endif // SIMPERTURBED_H
