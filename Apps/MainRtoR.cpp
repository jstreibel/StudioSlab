

#include "Studios/App/SafetyNet.h"

#include "Studios/Controller/Console/CLInterfaceSelector.h"

#include "Fields/Maps/RtoR/Controller/InteractingFormations/input-sym-oscillons.h"
#include "Fields/Maps/RtoR/Controller/InteractingFormations/input-general-oscillons.h"
#include "Fields/Maps/RtoR/Controller/SingleFormations/input-perturbed.h"
#include "Fields/Maps/RtoR/Controller/SingleFormations/input-shockwave.h"
#include "Fields/Maps/RtoR/Controller/SingleFormations/InputSingleOscillon.h"
#include "Fields/Maps/RtoR/App/SimulationsAppRtoR.h"

auto main(int argc, const char **argv) -> int {

    auto &im = CLInterfaceSelector::getInstance();

    /* sim 0 */im.registerBCInterface(new RtoR::InputSymmetricOscillon);
    /* sim 1 */im.registerBCInterface(new RtoR::InputPerturbations);
    /* sim 2 */im.registerBCInterface(new RtoR::InputGeneralOscillons);
    /* sim 3 */im.registerBCInterface(new RtoR::InputShockwave);
    /* sim 4 */im.registerBCInterface(new RtoR::InputSingleOscillon);

    im.setup(argc, argv);

    auto *prog = new SimulationsAppRtoR(argc, argv);

    return SafetyNet::jump(argc, argv, *prog);
}


