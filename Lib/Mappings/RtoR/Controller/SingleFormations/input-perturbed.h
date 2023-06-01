#ifndef SIMPERTURBED_H
#define SIMPERTURBED_H

#include "Mappings/RtoR/Controller/RtoRBCInterface.h"


namespace RtoR {

    class InputPerturbations : public RtoRBCInterface {
        DoubleParameter l   = DoubleParameter{1., "l", "Initial condition scale factor."};
        DoubleParameter eps = DoubleParameter{1., "a", "Perturbation value. a=1 means no perturbation."};

    public:
        InputPerturbations();

        auto getBoundary() const -> const void * override;
    };
}

#endif // SIMPERTURBED_H
