//
// Created by joao on 04/04/2020.
//

#ifndef V_SHAPE_INPUTSTATISTICAL_H
#define V_SHAPE_INPUTSTATISTICAL_H

#include "Models/KleinGordon/RtoR/KG-RtoRBuilder.h"

namespace RtoR {
    class InputStatistical  : public Builder {
        RealParameter E = RealParameter{1., "E", "System energy"};
        IntegerParameter n = IntegerParameter{100, "n", "Number of initial oscillons"};

    public:
        InputStatistical();

        auto getBoundary() -> void * override;
        static auto getDetailedDescription() -> Str;
    };
}


#endif //V_SHAPE_INPUTSTATISTICAL_H