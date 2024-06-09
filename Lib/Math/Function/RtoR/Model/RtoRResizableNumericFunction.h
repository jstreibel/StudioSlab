//
// Created by joao on 16/04/2021.
//

#ifndef V_SHAPE_RTORFUNCARBRESIZABLE_H
#define V_SHAPE_RTORFUNCARBRESIZABLE_H


#include "Math/Function/NumericFunction.h"
#include "RtoRFunction.h"

namespace Slab::Math::RtoR {

    class ResizableNumericFunction : public Base::FunctionT<Real, Real> {
        Real yMin=1, yMax=-1;
    public:
        auto getYMin() const -> Real;
        auto getYMax() const -> Real;


        Vector<Real> X;
        Real xMin, xMax;

        ResizableNumericFunction() : xMin(0), xMax(0), X(0) {};
        ResizableNumericFunction(Real xMin, Real xMax);

        auto operator()(Real x) const -> Real override;
        auto operator=(const ResizableNumericFunction &func) -> ResizableNumericFunction& {
            X = func.X;
            xMin = func.xMin;
            xMax = func.xMax;

            return *this;
        }

        void insertBack(Real y);

    };
}


#endif //V_SHAPE_RTORFUNCARBRESIZABLE_H
