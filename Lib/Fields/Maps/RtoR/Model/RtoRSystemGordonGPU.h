//
// Created by joao on 27/09/2019.
//

#ifndef V_SHAPE_RTORSYSTEMGORDONGPU_H
#define V_SHAPE_RTORSYSTEMGORDONGPU_H

#include "Studios/Util/Workaround/ABIDef.h"
#include "Studios/Math/System/System.h"
#include "RtoRFieldState.h"
#include "Studios/CoreMath/Util/device-config.h"

namespace RtoR {

    class SystemGordonGPU : Base::System<FieldState> {
    public:
        SystemGordonGPU(PosInt N);
        FieldState &dtF(const FieldState &in, FieldState &out, Real t, Real dt) override;

    private:
        DeviceVector temp;
    };

}


#endif //V_SHAPE_RTORSYSTEMGORDONGPU_H
