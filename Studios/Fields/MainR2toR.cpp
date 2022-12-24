//
// Created by joao on 10/8/21.
//

#include <Studios/App/CrashPad.h>

#include <Studios/Controller/Interface/InterfaceSelector.h>

#include <Fields/Mappings/R2toR/Controller/R2ToRInputShockwave.h>
#include "R2toR/GrowingHole/GrowingHoleInput.h"
#include "R2toR/DiracSpeedInput.h"
#include "R2toR/LeadingDelta/LeadingDeltaInput.h"
//#include <Fields/Mappings/R2toR/Controller/R2ToRInputShockwaveAtT0.h>
//#include <Fields/Mappings/R2toR/Controller/R2ToRInputRadialShockwave.h>

#include <Fields/Mappings/R2toR/App/SimulationsAppR2ToR.h>


int main(int argc, const char **argv) {

    try {
        auto &im = InterfaceSelector::getInstance();

        /* sim 0 */ im.registerOption(new R2toR::R2toRInputShockwave());
        /* sim 1 */ im.registerOption(new R2toR::InputGrowingHole());
        /* sim 2 */ im.registerOption(new R2toR::InputDiracSpeed());
        /* sim 2 */ im.registerOption(new R2toR::LeadingDeltaInput());


        // /* sim 1 */im.registerCandidate(new R2toR::R2toRInputShockwaveAt_t0);
        // /* sim 2 */im.registerCandidate(new R2toR::R2toRInputRadialShockwave);

        im.setup(argc, argv);

        auto prog = SimulationsAppR2toR(argc, argv);

        return SafetyNet::jump(prog);

    }

    catch (const char *e) {
        std::cout << "\n\nUnhandled Exception reached the top of main: " << "\033[91m\033[1m"
                  << e << "\033[0m" << ", application will now exit" << std::endl;
        return -1;
    }


}


