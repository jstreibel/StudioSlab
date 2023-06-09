//
// Created by joao on 23/09/2019.
//

#ifndef V_SHAPE_R2TORINPUTSHOCKWAVE_H
#define V_SHAPE_R2TORINPUTSHOCKWAVE_H

#include "Mappings/SimulationBuilder.h"

#include "Common/NativeTypes.h"
#include "Phys/Numerics/Output/Plugs/Plug.h"
#include "R2ToRBCInterface.h"

namespace R2toR {

    class R2toRInputShockwave : public R2toRBCInterface {
    private:
        RealParameter eps   =RealParameter{1., "eps", "Quasi-shockwave 'epsilon' parameter."};
        RealParameter theta =RealParameter{0., "theta", "Ellipse rotation."};
        RealParameter E     =RealParameter{1., "E", "Total energy."};
        RealParameter e     =RealParameter{0., "e", "Eccentricity."};
        RealParameter t0    =RealParameter{0., "t0", "Initial time."};
    public:
        R2toRInputShockwave();
        auto getBoundary() const -> const void * override;
    };

}


#endif //V_SHAPE_R2TORINPUTSHOCKWAVE_H
