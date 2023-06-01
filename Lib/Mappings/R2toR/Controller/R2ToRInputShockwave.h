//
// Created by joao on 23/09/2019.
//

#ifndef V_SHAPE_R2TORINPUTSHOCKWAVE_H
#define V_SHAPE_R2TORINPUTSHOCKWAVE_H

#include "Mappings/BCInterface.h"

#include "Common/NativeTypes.h"
#include "Phys/Numerics/Output/Channel/OutputChannel.h"
#include "R2ToRBCInterface.h"

namespace R2toR {

    class R2toRInputShockwave : public R2toRBCInterface {
    private:
        DoubleParameter eps   =DoubleParameter{1., "eps", "Quasi-shockwave 'epsilon' parameter."};
        DoubleParameter theta =DoubleParameter{0., "theta", "Ellipse rotation."};
        DoubleParameter E     =DoubleParameter{1., "E", "Total energy."};
        DoubleParameter e     =DoubleParameter{0., "e", "Eccentricity."};
        DoubleParameter t0    =DoubleParameter{0., "t0", "Initial time."};
    public:
        R2toRInputShockwave();
        auto getBoundary() const -> const void * override;
    };

}


#endif //V_SHAPE_R2TORINPUTSHOCKWAVE_H
