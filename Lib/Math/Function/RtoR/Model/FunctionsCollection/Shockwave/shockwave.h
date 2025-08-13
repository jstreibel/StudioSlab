#ifndef SHOCKWAVE_H
#define SHOCKWAVE_H

#include "Math/Function/RtoR/Model/RtoRFunction.h"

namespace Slab::Math::RtoR {
    class ShockwavePhi : public RtoR::Function {
    public:
        ShockwavePhi(DevFloat h);

        DevFloat operator()(DevFloat x) const override;

    private:
        DevFloat h;
    };

    class ShockwaveDPhiDt : public RtoR::Function {
    public:
        ShockwaveDPhiDt(DevFloat h);

        DevFloat operator()(DevFloat x) const override;

    private:
        DevFloat h;
    };
}

#endif // SHOCKWAVE_H
