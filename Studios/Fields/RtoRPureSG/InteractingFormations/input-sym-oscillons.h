#ifndef SIMGENERALOSCILLON_H
#define SIMGENERALOSCILLON_H

#include "Models/KleinGordon/RtoR/KG-RtoRBuilder.h"

namespace RtoR {
    class InputSymmetricOscillon : public Builder {
        RealParameter v               = RealParameter(.0, "v", "Each oscillon's border speed.");
        RealParameter V               = RealParameter(.0, "V", "Each oscillon's speed.");
        RealParameter alpha           = RealParameter(.0, "alpha", "Each oscillon's phase.");
        BoolParameter isAntiSymmetric = BoolParameter(false, "asym,a", "Choose with anti-symmetrical oscillons.");

    public:
        InputSymmetricOscillon();

        auto getBoundary() -> void * override;
    };
}

#endif // SIMGENERALOSCILLON_H