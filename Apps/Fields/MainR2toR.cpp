//
// Created by joao on 10/8/21.
//

#include <Studios/App/CrashPad.h>

#include <Studios/Controller/Interface/InterfaceSelector.h>

#include <Fields/Mappings/R2toR/Controller/R2ToRInputShockwave.h>
#include <Fields/Mappings/R2toR/Controller/R2ToRInputShockwaveAtT0.h>
#include <Fields/Mappings/R2toR/Controller/R2ToRInputRadialShockwave.h>

#include <Fields/Mappings/R2toR/App/SimulationsAppR2ToR.h>


int main(int argc, const char **argv) {

    auto &im = InterfaceSelector::getInstance();

    // /* sim 0 */im.registerCandidate(new R2toR::R2toRInputShockwave);
    // /* sim 1 */im.registerCandidate(new R2toR::R2toRInputShockwaveAt_t0);
    // /* sim 2 */im.registerCandidate(new R2toR::R2toRInputRadialShockwave);

    auto prog = SimulationsAppR2toR(argc, argv);

    return SafetyNet::jump(argc, argv, prog);
}


