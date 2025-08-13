//
// Created by joao on 12/09/23.
//

#ifndef STUDIOSLAB_BYPASS_H
#define STUDIOSLAB_BYPASS_H

#include "Math/Function/RtoR/Model/RtoRFunction.h"

namespace Slab::Math::RtoR {

    class Bypass : public Function {
    public:
        inline DevFloat operator()(DevFloat x) const override { return x; };
    };

} // RtoR

#endif //STUDIOSLAB_BYPASS_H
