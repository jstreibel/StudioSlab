//
// Created by joao on 11/09/23.
//

#ifndef STUDIOSLAB_FOURIERMODES_H
#define STUDIOSLAB_FOURIERMODES_H

#include "Maps/RtoR/Model/RtoRFunction.h"
#include "Function.h"

namespace RtoC {

    class FourierModes : public Function {
        RtoR::Function::Ptr baseFunc;

        Real xMin, L;
        Count samples;

    public:
        explicit FourierModes(RtoR::Function::Ptr baseFunc, Real xMin=0, Real L=1, Count samples=500);

        void set_xMin(Real val);
        void setL(Real L);
        void setNSamples(Count N);

        Complex operator()(Real k) const override;

    };

} // RtoR

#endif //STUDIOSLAB_FOURIERMODES_H
