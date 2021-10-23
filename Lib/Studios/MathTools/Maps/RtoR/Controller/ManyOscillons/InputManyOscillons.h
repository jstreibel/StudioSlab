//
// Created by joao on 04/04/2020.
//

#ifndef V_SHAPE_INPUTMANYOSCILLONS_H
#define V_SHAPE_INPUTMANYOSCILLONS_H

#include <Studios/MathTools/Maps/RtoR/Controller/RtoRBCInterface.h>

namespace RtoR {
    class InputManyOscillons  : public RtoRBCInterface {

    public:
        InputManyOscillons();
        auto getBoundary() const -> const void * override = 0;

    };
}


#endif //V_SHAPE_INPUTMANYOSCILLONS_H
