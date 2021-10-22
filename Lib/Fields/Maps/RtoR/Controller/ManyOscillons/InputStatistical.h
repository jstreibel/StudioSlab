//
// Created by joao on 04/04/2020.
//

#ifndef V_SHAPE_INPUTSTATISTICAL_H
#define V_SHAPE_INPUTSTATISTICAL_H

#include <Lib/Fields/Maps/RtoR/Controller/RtoRBCInterface.h>

namespace RtoR {
    class InputStatistical  : public RtoRBCInterface {
        DoubleParameter E = DoubleParameter{1., "E", "System energy"};
        IntegerParameter n = IntegerParameter{100, "n", "Number of initial oscillons"};

    public:
        InputStatistical();

        auto getBoundary() const -> const void * override = 0;
        static auto getDetailedDescription() -> String;
    };
}


#endif //V_SHAPE_INPUTSTATISTICAL_H
