#ifndef SHOCKWAVE_H
#define SHOCKWAVE_H

#include "../../Model/RtoRFunction.h"

namespace RtoR {
    class ShockwavePhi : public Function {
    public:
        ShockwavePhi(Real h);

        Real operator()(Real x) const override;

    private:
        Real h;
    };

    class ShockwaveDPhiDt : public Function {
    public:
        ShockwaveDPhiDt(Real h);

        Real operator()(Real x) const override;

    private:
        Real h;
    };
}

#endif // SHOCKWAVE_H
