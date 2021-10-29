

#include "Studios/App/SafetyNet.h"

#include "Studios/Controller/Console/CLInterfaceSelector.h"

#include <Fields/Mappings/RtoR/Controller/InteractingFormations/input-sym-oscillons.h>
#include <Fields/Mappings/RtoR/Controller/InteractingFormations/input-general-oscillons.h>
#include <Fields/Mappings/RtoR/Controller/SingleFormations/input-perturbed.h>
#include <Fields/Mappings/RtoR/Controller/SingleFormations/input-shockwave.h>
#include <Fields/Mappings/RtoR/Controller/SingleFormations/InputSingleOscillon.h>
#include <Fields/Mappings/RtoR/App/FieldsApp-RtoR.h>

auto main(int argc, const char **argv) -> int {

    auto &im = CLInterfaceSelector::getInstance();

    /* sim 0 */im.registerCandidate(new RtoR::InputSymmetricOscillon);
    /* sim 1 */im.registerCandidate(new RtoR::InputPerturbations);
    /* sim 2 */im.registerCandidate(new RtoR::InputGeneralOscillons);
    /* sim 3 */im.registerCandidate(new RtoR::InputShockwave);
    /* sim 4 */im.registerCandidate(new RtoR::InputSingleOscillon);

    im.setup(argc, argv);

    auto *prog = new SimulationsAppRtoR(argc, argv);

    return SafetyNet::jump(argc, argv, *prog);
}


