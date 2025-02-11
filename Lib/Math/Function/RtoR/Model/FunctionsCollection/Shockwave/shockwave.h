#ifndef SHOCKWAVE_H
#define SHOCKWAVE_H

#include "Math/Function/RtoR/Model/RtoRFunction.h"

namespace Slab::Math::RtoR {
    class ShockwavePhi : public RtoR::Function {
    public:
        ShockwavePhi(Real h);

        Real operator()(Real x) const override;

    private:
        Real h;
    };

    class ShockwaveDPhiDt : public RtoR::Function {
    public:
        ShockwaveDPhiDt(Real h);

        Real operator()(Real x) const override;

    private:
        Real h;
    };
}

#endif // SHOCKWAVE_H
