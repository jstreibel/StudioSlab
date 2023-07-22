//
// Created by joao on 23/09/2019.
//

#ifndef V_SHAPE_R2TORINPUTSHOCKWAVE_H
#define V_SHAPE_R2TORINPUTSHOCKWAVE_H

#include "Phys/Numerics/VoidBuilder.h"

#include "Common/NativeTypes.h"
#include "Phys/Numerics/Output/Plugs/Socket.h"
#include "Mappings/R2toR/Controller/R2ToRSimBuilder.h"

namespace R2toR {

    class R2toRInputShockwave : public R2toR::Simulation::Builder {
    private:
        RealParameter::Ptr eps   = RealParameter::New(1., "eps", "Quasi-shockwave 'epsilon' parameter.");
        RealParameter::Ptr theta = RealParameter::New(0., "theta", "Ellipse rotation.");
        RealParameter::Ptr E     = RealParameter::New(1., "E", "Total energy.");
        RealParameter::Ptr e     = RealParameter::New(0., "e", "Eccentricity.");
        RealParameter::Ptr t0    = RealParameter::New(0., "t0", "Initial time.");
    public:
        R2toRInputShockwave();
        auto getBoundary() -> void * override;
    };

}


#endif //V_SHAPE_R2TORINPUTSHOCKWAVE_H
