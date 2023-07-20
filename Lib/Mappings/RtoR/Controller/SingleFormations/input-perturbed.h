#ifndef SIMPERTURBED_H
#define SIMPERTURBED_H

#include "Mappings/RtoR/Controller/RtoRBCInterface.h"


namespace RtoR {

    class InputPerturbations : public RtoRBCInterface {
        RealParameter l   = RealParameter{1., "l", "Initial condition scale factor."};
        RealParameter eps = RealParameter{1., "a", "Perturbation value. a=1 means no perturbation."};

    public:
        InputPerturbations();

        auto getBoundary() -> void * override;
    };
}

#endif // SIMPERTURBED_H
