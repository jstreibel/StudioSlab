

#include "Base/App/CrashPad.h"
#include "Base/Controller/Interface/InterfaceSelector.h"

#include "Mappings/RtoR/Controller/InteractingFormations/input-sym-oscillons.h"
#include "Mappings/RtoR/Controller/InteractingFormations/input-general-oscillons.h"
#include "Mappings/RtoR/Controller/SingleFormations/input-perturbed.h"
#include "Mappings/RtoR/Controller/SingleFormations/input-shockwave.h"
#include "Mappings/RtoR/Controller/SingleFormations/InputSingleOscillon.h"
#include "Mappings/RtoR/App/FieldsApp-RtoR.h"

int run(int argc, const char **argv) {
    InterfaceSelector selector("Simulation builder selector");

    auto option1 = new RtoR::InputSymmetricOscillon();
    auto option2 = new RtoR::InputPerturbations();
    auto option3 = new RtoR::InputGeneralOscillons();
    auto option4 = new RtoR::InputShockwave();
    auto option5 = new RtoR::InputSingleOscillon();

    /* sim 0 */selector.registerOption(option1->getInterface());
    /* sim 1 */selector.registerOption(option2->getInterface());
    /* sim 2 */selector.registerOption(option3->getInterface());
    /* sim 3 */selector.registerOption(option4->getInterface());
    /* sim 4 */selector.registerOption(option5->getInterface());

    auto selectedInterface = selector.preParse(argc, argv).getCurrentCandidate();
    auto input    = dynamic_cast<Base::Simulation::VoidBuilder*>(selectedInterface->getOwner());
    auto inputPtr = Base::Simulation::VoidBuilder::Ptr(input);

    auto prog = SimulationsAppRtoR(argc, argv, inputPtr);

    return prog.run();
}

auto main(int argc, const char **argv) -> int {
    return SafetyNet::jump(run, argc, argv);
}


