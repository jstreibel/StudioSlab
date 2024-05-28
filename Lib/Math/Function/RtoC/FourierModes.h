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

        Real xMin=0, L=1;
        Count samples=500;

    public:
        explicit FourierModes(RtoR::Function_ptr baseFunc, Real xMin, Real L, Count samples);

        void setBaseFunction(RtoR::Function_ptr func);
        void set_xMin(Real val);
        void setL(Real L);
        void setNSamples(Count N);

        Complex operator()(Real k) const override;

    };

} // RtoR

#endif //STUDIOSLAB_FOURIERMODES_H
