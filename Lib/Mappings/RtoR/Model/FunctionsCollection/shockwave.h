#ifndef SHOCKWAVE_H
#define SHOCKWAVE_H

#include "Mappings/RtoR/Model/RtoRFunction.h"

namespace RtoR {
    class ShockwavePhi : public FunctionT {
    public:
        ShockwavePhi(Real h);

        Real operator()(Real x) const override;

    private:
        Real h;
    };

    class ShockwaveDPhiDt : public FunctionT {
    public:
        ShockwaveDPhiDt(Real h);

        Real operator()(Real x) const override;

    private:
        Real h;
    };
}

#endif // SHOCKWAVE_H
