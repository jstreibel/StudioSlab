//
// Created by joao on 12/09/23.
//

#ifndef STUDIOSLAB_R2TOCDISCRETEFUNCTION_H
#define STUDIOSLAB_R2TOCDISCRETEFUNCTION_H

#include "Math/Function/DiscreteFunction.h"

namespace R2toC {

    class DiscreteFunction : public Core::FunctionT<Real2D, Complex>  {
    public:
        Complex operator()(Real2D x) const override;
    };

} // R2toC

#endif //STUDIOSLAB_R2TOCDISCRETEFUNCTION_H
