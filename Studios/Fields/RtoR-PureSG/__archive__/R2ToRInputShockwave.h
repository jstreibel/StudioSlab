//
// Created by joao on 23/09/2019.
//

#ifndef V_SHAPE_R2TORINPUTSHOCKWAVE_H
#define V_SHAPE_R2TORINPUTSHOCKWAVE_H

#include "Math/Numerics/NumericalRecipe.h"

#include "Utils/NativeTypes.h"
#include "Math/Numerics/Socket.h"
#include "Models/KleinGordon/R2toR/R2toRBuilder.h"

namespace Slab::Math::R2toR {

    class R2toRInputShockwave : public R2toR::Builder {
    private:
        TPointer<RealParameter> eps   = New<RealParameter>(1., "eps", "Quasi-shockwave 'epsilon' parameter.");
        TPointer<RealParameter> theta = New<RealParameter>(0., "theta", "Ellipse rotation.");
        TPointer<RealParameter> E     = New<RealParameter>(1., "E", "Total energy.");
        TPointer<RealParameter> e     = New<RealParameter>(0., "e", "Eccentricity.");
        TPointer<RealParameter> t0    = New<RealParameter>(0., "t0", "Initial time.");
    public:
        R2toRInputShockwave();
        auto getBoundary() -> void * override;
    };

}


#endif //V_SHAPE_R2TORINPUTSHOCKWAVE_H
