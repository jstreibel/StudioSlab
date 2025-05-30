//
// Created by joao on 11/05/24.
//

#ifndef STUDIOSLAB_COMPLEXMAGNITUDEFUNCTION_H
#define STUDIOSLAB_COMPLEXMAGNITUDEFUNCTION_H

#include "Math/Function/R2toR/Model/R2toRFunction.h"
#include "Math/Function/R2toC/R2toCNumericFunction.h"

namespace Slab::Math::R2toR {

    class ComplexMagnitudeFunction : public R2toR::Function {
        std::shared_ptr<R2toC::NumericFunction> function = nullptr;

    public:
        ComplexMagnitudeFunction();
        ComplexMagnitudeFunction(std::shared_ptr<R2toC::NumericFunction>);

        void setFunc(std::shared_ptr<R2toC::NumericFunction>);

        Real operator()(Real2D x) const override;
    };

} // R2toR

#endif //STUDIOSLAB_COMPLEXMAGNITUDEFUNCTION_H
