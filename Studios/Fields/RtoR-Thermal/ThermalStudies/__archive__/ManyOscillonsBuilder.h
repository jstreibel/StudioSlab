//
// Created by joao on 04/04/2020.
//

#ifndef V_SHAPE_INPUTMANYOSCILLONS_H
#define V_SHAPE_INPUTMANYOSCILLONS_H

#include "Models/KleinGordon/RtoR/KG-RtoRBuilder.h"

namespace RtoR {
    class ManyOscillonsBuilder  : public Builder {

        IntegerParameter nOscillons = IntegerParameter(100, "n_osc", "Number of oscillons distributed.");
    public:
        ManyOscillonsBuilder();
        auto getBoundary() -> void * override;

    protected:
        auto buildOpenGLOutput() -> Monitor * override;

    };
}


#endif //V_SHAPE_INPUTMANYOSCILLONS_H
