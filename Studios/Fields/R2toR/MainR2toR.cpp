//
// Created by joao on 10/8/21.
//

#include "Base/App/CrashPad.h"

#include "Base/Controller/Interface/InterfaceSelector.h"

#include "Mappings/R2toR/Controller/R2ToRInputShockwave.h"
#include "GrowingHole/GrowingHoleInput.h"
#include "DiracSpeedInput.h"
#include "LeadingDelta/LeadingDelta.h"

#include "Mappings/R2toR/App/SimulationsAppR2ToR.h"


int main(int argc, const char **argv) {

    try {
        auto &im = InterfaceSelector::getInstance();

        ///* sim 0 */ im.registerOption(new R2toR::R2toRInputShockwave());
        /* sim 0 */ im.registerOption( new R2toR :: DiracSpeed   :: Input( ) );
        /* sim 1 */ im.registerOption( new R2toR :: GrowingHole  :: Input( ) );
        /* sim 2 */ im.registerOption( new R2toR :: LeadingDelta :: Input( ) );


        // /* sim 1 */im.registerCandidate(new R2toR::R2toRInputShockwaveAt_t0);
        // /* sim 2 */im.registerCandidate(new R2toR::R2toRInputRadialShockwave);

        im.preParse(argc, argv);

        auto prog = SimulationsAppR2toR(argc, argv);

        return SafetyNet::jump(prog);

    } catch (const char *e) {
        std::cout << "\n\nUnhandled Exception reached the top of main: " << "\033[91m\033[1m"
                  << e << "\033[0m" << ", application will now exit" << std::endl;
        return -1;
    } catch (const std::string e) {
        std::cout << "\n\nUnhandled Exception reached the top of main: " << "\033[91m\033[1m"
                  << e << "\033[0m" << ", application will now exit" << std::endl;
        return -1;
    }


}


