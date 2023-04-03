

#include "Studios/App/CrashPad.h"

#include "Studios/Controller/Interface/InterfaceSelector.h"

#include "Fields/Mappings/RtoR/Controller/InteractingFormations/input-sym-oscillons.h"
#include "Fields/Mappings/RtoR/Controller/InteractingFormations/input-general-oscillons.h"
#include "Fields/Mappings/RtoR/Controller/SingleFormations/input-perturbed.h"
#include "Fields/Mappings/RtoR/Controller/SingleFormations/input-shockwave.h"
#include "Fields/Mappings/RtoR/Controller/SingleFormations/InputSingleOscillon.h"
#include "Fields/Mappings/RtoR/App/FieldsApp-RtoR.h"

auto main(int argc, const char **argv) -> int {

    auto &im = InterfaceSelector::getInstance();

    /* sim 0 */im.registerOption(new RtoR::InputSymmetricOscillon);
    /* sim 1 */im.registerOption(new RtoR::InputPerturbations);
    /* sim 2 */im.registerOption(new RtoR::InputGeneralOscillons);
    /* sim 3 */im.registerOption(new RtoR::InputShockwave);
    /* sim 4 */im.registerOption(new RtoR::InputSingleOscillon);

    im.preParse(argc, argv);

    auto *prog = new SimulationsAppRtoR(argc, argv);

    return SafetyNet::jump(*prog);
}


