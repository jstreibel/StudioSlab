//
// Created by joao on 11/4/21.
//

#ifndef STUDIOSLAB_XYAPP_H
#define STUDIOSLAB_XYAPP_H

#include "AppBase.h"
#include "../../Lib/Core/Controller/Parameter/BuiltinParameters.h"

namespace XY {

    class App : public Slab::Core::AppBase {
        Slab::Core::IntegerParameter N =         {20,   Slab::Core::FParameterDescription{'N', "sqrtN", "Sqrt of number of sites"}};
        Slab::Core::IntegerParameter MCSteps =   {7000, Slab::Core::FParameterDescription{'S', "MCS", "Total Monte Carlo steps to simulate"}};
        Slab::Core::IntegerParameter transient = {1000, Slab::Core::FParameterDescription{"transient", "Transient guess to reach equilibrium"}};
        Slab::Core::RealParameter    T =         {1.0,  Slab::Core::FParameterDescription{'T', "temperature", "Temperature"}};

    public:

        App(int argc, const char **argv);

        int run() override;

    };

}


#endif //STUDIOSLAB_XYAPP_H
