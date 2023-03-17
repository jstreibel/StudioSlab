//
// Created by joao on 3/17/23.
//

#ifndef STUDIOSLAB_MONTECARLO_H
#define STUDIOSLAB_MONTECARLO_H


#include "Fields/Mappings/RtoR/Controller/RtoRBCInterface.h"

#include "RtoROutGLMontecarlo.h"

namespace Montecarlo {


    class OutputBuilder : public OutputStructureBuilderRtoR {
    protected:
        auto buildOpenGLOutput() -> RtoR::OutputOpenGL * override { return new Montecarlo::OutGL( ); }
    };


    class Input : public RtoRBCInterface {
        DoubleParameter T = DoubleParameter(1.e4, "Temperature,T", "System temperature");
        DoubleParameter E = DoubleParameter{1., "E", "System energy"};
        IntegerParameter n = IntegerParameter{100, "n", "Number of initial oscillons"};

    public:
        Input()
        : RtoRBCInterface("Input of initial field data for Montecarlo simulation.", "Montecarlo",
                          new Montecarlo::OutputBuilder()) { addParameters({&T, &E, &n}); }

    private:
        auto getBoundary() const -> const void * override;

    };




}


#endif //STUDIOSLAB_MONTECARLO_H
