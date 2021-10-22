//
// Created by joao on 10/8/21.
//

#include "Lib/App/SafetyNet.h"

#include "Apps/Maps/R2toR/Controller/R2ToRInputShockwave.h"
#include "Apps/Maps/R2toR/Controller/R2ToRInputShockwaveAtT0.h"
#include "Apps/Maps/R2toR/Controller/R2ToRInputRadialShockwave.h"

#include "Apps/Maps/R2toR/App/SimulationsAppR2ToR.h"


int main(int argc, const char **argv) {

    auto &im = CLInterfaceSelector::getInstance();

    /* sim 0 */im.registerInterface(new R2toR::R2toRInputShockwave);
    /* sim 1 */im.registerInterface(new R2toR::R2toRInputShockwaveAt_t0);
    /* sim 2 */im.registerInterface(new R2toR::R2toRInputRadialShockwave);

    auto prog = SimulationsAppR2toR(argc, argv);

    return SafetyNet::jump(argc, argv, prog);
}


