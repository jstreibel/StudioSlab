//
// Created by joao on 26/03/25.
//

#ifndef R2TOR_NULL_FUNCTION_H
#define R2TOR_NULL_FUNCTION_H

#include <Math/Function/R2toR/Model/R2toRFunction.h>

namespace Slab::Math::R2toR {

    class NullFunction : public Function{
    public:
        double operator()(Real2D x) const override { return 0; };
    };

}
#endif //R2TOR_NULL_FUNCTION_H
