//
// Created by joao on 20/11/2019.
//

#ifndef V_SHAPE_R2TORINPUTRADIALSHOCKWAVE_H
#define V_SHAPE_R2TORINPUTRADIALSHOCKWAVE_H

#include "Math/Numerics/VoidBuilder.h"

namespace R2toR {
    class R2toRInputRadialShockwave : public Base::BCInterface {
    private:
        //DoubleParameter a = DoubleParameter{1., "a", "Shockwave simulation delta \'height\' scale parameter."};
        //DoubleParameter eps = DoubleParameter{1., "epsilon", "Shockwave simulation delta \'width\' parameter."};
        DoubleParameter t0 = DoubleParameter{0., "t0", "Shockwave initial time."};

    public:
        R2toRInputRadialShockwave();

        void YoureTheChosenOne(CLVariablesMap &vm) override;

        auto getGeneralDescription() const -> String override;
    };
}

#endif //V_SHAPE_R2TORINPUTRADIALSHOCKWAVE_H
