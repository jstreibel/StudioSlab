#ifndef SIMPERTURBED_H
#define SIMPERTURBED_H

#include "../Builder.h"


namespace Studios::PureSG {

    using namespace Slab::Core;


    class InputPerturbations : public Builder {
        RealParameter l   = RealParameter{1., "l", "Initial condition scale factor."};
        RealParameter eps = RealParameter{1., "a", "Perturbation value. a=1 means no perturbation."};

    public:
        InputPerturbations();

        auto GetBoundary() -> Math::Base::BoundaryConditions_ptr override;
    };
}

#endif // SIMPERTURBED_H
