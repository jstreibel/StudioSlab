#ifndef SIMGENERALOSCILLON_H
#define SIMGENERALOSCILLON_H

#include "Models/KleinGordon/RtoR/KG-RtoRBuilder.h"
#include "../Builder.h"

namespace Slab::Math::RtoR {
class InputSymmetricOscillon : public PureSG::Builder {
        RealParameter v               = RealParameter(.0, "v", "Each oscillon's border speed");
        RealParameter V               = RealParameter(.0, "u", "Each oscillon's speed");
        RealParameter alpha           = RealParameter(.0, "alpha", "Each oscillon's phase");
        BoolParameter isAntiSymmetric = BoolParameter(false, "asym,a", "Choose with anti-symmetrical oscillons");

    public:
        InputSymmetricOscillon();

        auto getBoundary() -> void * override;


    };
}

#endif // SIMGENERALOSCILLON_H
