//
// Created by joao on 11/09/23.
//

#ifndef STUDIOSLAB_FOURIERMODES_H
#define STUDIOSLAB_FOURIERMODES_H

#include "Math/Function/RtoR/Model/RtoRFunction.h"
#include "Function.h"

namespace Slab::Math::RtoC {

    class FourierModes : public Function {
        RtoR::Function_ptr baseFunc;

        DevFloat xMin=0, L=1;
        CountType samples=500;

    public:
        explicit FourierModes(RtoR::Function_ptr baseFunc, DevFloat xMin, DevFloat L, CountType samples);

        void setBaseFunction(RtoR::Function_ptr func);
        void set_xMin(DevFloat val);
        void setL(DevFloat L);
        void setNSamples(CountType N);

        Complex operator()(DevFloat k) const override;

    };

} // RtoR

#endif //STUDIOSLAB_FOURIERMODES_H
