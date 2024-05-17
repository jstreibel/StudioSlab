

#include "Core/App/CrashPad.h"
#include "Core/Controller/Interface/InterfaceSelector.h"

#include "Math/App.h"

#include "InteractingFormations/input-sym-oscillons.h"
#include "InteractingFormations/input-general-oscillons.h"
#include "SingleFormations/input-perturbed.h"
#include "SingleFormations/input-shockwave.h"
#include "SingleFormations/InputSingleOscillon.h"

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
    auto input    = dynamic_cast<RtoR::KGBuilder*>(selectedInterface->getOwner());
    auto inputPtr = RtoR::KGBuilder::Ptr(input);

    auto prog = Simulation::App(argc, argv, inputPtr);

    return prog.run();
}

auto main(int argc, const char **argv) -> int {
    return SafetyNet::jump(run, argc, argv);
}


