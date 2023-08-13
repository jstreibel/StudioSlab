//
// Created by joao on 11/4/21.
//

#ifndef STUDIOSLAB_XYAPP_H
#define STUDIOSLAB_XYAPP_H

#include "Base/App/AppBase.h"
#include "Base/Controller/Interface/CommonParameters.h"

namespace XY {

    class App : public AppBase {

        IntegerParameter N = {20, "N,sqrtN", "Sqrt of number of sites"};
        IntegerParameter MCSteps = {7000, "S,MCS", "Total Monte Carlo steps to simulate"};
        IntegerParameter transient = {1000, "transient", "Transient guess to reach equilibrium"};
        RealParameter T = {1.0, "T,temperature", "Temperature"};

    public:

        App(int argc, const char **argv);

        int run() override;

    };

}


#endif //STUDIOSLAB_XYAPP_H
