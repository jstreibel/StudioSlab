#ifndef SIMPERTURBED_H
#define SIMPERTURBED_H

#include "Models/KleinGordon/RtoR/KG-RtoRBuilder.h"


namespace RtoR {

    class InputPerturbations : public Builder {
        RealParameter l   = RealParameter{1., "l", "Initial condition scale factor."};
        RealParameter eps = RealParameter{1., "a", "Perturbation value. a=1 means no perturbation."};

    public:
        InputPerturbations();

        auto getBoundary() -> void * override;
    };
}

#endif // SIMPERTURBED_H
