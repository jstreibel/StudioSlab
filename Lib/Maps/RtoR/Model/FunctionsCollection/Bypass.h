//
// Created by joao on 12/09/23.
//

#ifndef STUDIOSLAB_BYPASS_H
#define STUDIOSLAB_BYPASS_H

#include "Maps/RtoR/Model/RtoRFunction.h"

namespace RtoR {

    class Bypass : public Function {
    public:
        inline Real operator()(Real x) const override { return x; };
    };

} // RtoR

#endif //STUDIOSLAB_BYPASS_H
