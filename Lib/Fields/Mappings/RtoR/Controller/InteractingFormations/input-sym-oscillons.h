#ifndef SIMGENERALOSCILLON_H
#define SIMGENERALOSCILLON_H


#include "../../Controller/RtoRBCInterface.h"

namespace RtoR {
    class InputSymmetricOscillon : public RtoRBCInterface {
        DoubleParameter v             = DoubleParameter{.0, "v", "Each oscillon's border speed."};
        DoubleParameter V             = DoubleParameter{.0, "V", "Each oscillon's speed."};
        DoubleParameter alpha         = DoubleParameter{.0, "alpha", "Each oscillon's phase."};
        BoolParameter isAntiSymmetric = BoolParameter{false, "asym,a", "Choose with anti-symmetrical oscillons."};

    public:
        InputSymmetricOscillon();

        auto getBoundary() const -> const void * override;
    };
}

#endif // SIMGENERALOSCILLON_H
