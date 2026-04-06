#ifndef SIMSHOCKWAVE_H
#define SIMSHOCKWAVE_H

#include "../Builder.h"

namespace Studios::PureSG {
    class FInputShockwave : public FBuilder {
    private:

        unsigned int deltaType = 1;
        RealParameter a0 = RealParameter(1., FParameterDescription{"a0", "Shockwave discontinuity value."});
        RealParameter E  = RealParameter(1., FParameterDescription{'E', "Shockwave simulation initial (and total) energy."});

    public:
        FInputShockwave();

        auto GetBoundary() -> Math::Base::BoundaryConditions_ptr override;

    };

    using InputShockwave [[deprecated("Use FInputShockwave")]] = FInputShockwave;
}

#endif // SIMSHOCKWAVE_H
