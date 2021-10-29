//
// Created by joao on 16/04/2021.
//

#ifndef V_SHAPE_RTORFUNCARBRESIZABLE_H
#define V_SHAPE_RTORFUNCARBRESIZABLE_H


#include <Studios/CoreMath/Function/FunctionArbitrary.h>
#include "RtoRFunction.h"

namespace RtoR {
    class FuncArbResizable : public Base::Function<Real, Real> {
        Real yMin=1, yMax=-1;
    public:
        auto getYMin() const -> Real;
        auto getYMax() const -> Real;


        std::vector<Real> X;
        Real xMin, xMax;

        FuncArbResizable() : xMin(0), xMax(0), X(0) {};
        FuncArbResizable(Real xMin, Real xMax);

        auto operator()(Real x) const -> Real override;
        auto operator=(const FuncArbResizable &func) -> FuncArbResizable& {
            X = func.X;
            xMin = func.xMin;
            xMax = func.xMax;

            return *this;
        }

        void insertBack(Real y);

    };
}


#endif //V_SHAPE_RTORFUNCARBRESIZABLE_H
