//
// Created by joao on 24/06/2021.
//

#ifndef V_SHAPE_NOISEFUNCTION_H
#define V_SHAPE_NOISEFUNCTION_H

#include <Studios/Math/Maps/RtoR/Model/RtoRFunction.h>

namespace RtoR {
    class NoiseFunction : public Function {
        Real min, max;

    public:
        NoiseFunction(Real min, Real max);

        Real operator()(Real x) const override;

    };

}


#endif //V_SHAPE_NOISEFUNCTION_H
