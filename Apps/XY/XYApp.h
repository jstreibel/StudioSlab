//
// Created by joao on 11/4/21.
//

#ifndef STUDIOSLAB_XYAPP_H
#define STUDIOSLAB_XYAPP_H

#include <Studios/App/AppBase.h>

#include <Studios/Controller/Interface/CommonParameters.h>
#include <Studios/Controller/Interface/Interface.h>

class XYApp : public AppBase {

    IntegerParameter L = {20, "sqrtN", "Sqrt of number of sites"};
    IntegerParameter TSteps = {75, "TSteps", "Temperature steps between TMax and TMin"};
    IntegerParameter MCSteps = {7000, "MCS,s", "Total Monte Carlo steps to simulate"};
    IntegerParameter transient = {1000, "transient", "Transient guess to reach equilibrium"};
    DoubleParameter T = {1.0, "T,T", "Temperature"};

public:

    XYApp(int argc, const char **argv);

    int run() override;

};



#endif //STUDIOSLAB_XYAPP_H
