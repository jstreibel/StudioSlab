

#include "Core/App/CrashPad.h"
#include "Core/Controller/Interface/InterfaceSelector.h"

#include "Math/App.h"

#include "InteractingFormations/input-sym-oscillons.h"
#include "InteractingFormations/input-general-oscillons.h"
#include "SingleFormations/input-perturbed.h"
#include "SingleFormations/input-shockwave.h"
#include "SingleFormations/InputSingleOscillon.h"

int run(int argc, const char **argv) {
    using namespace Slab::Core;
    using namespace Slab::Math;
    using namespace Slab::Models::KGRtoR;

    InterfaceSelector selector("Simulation builder selector");

    auto option1 = new Studios::PureSG::InputSymmetricOscillon();
    auto option2 = new Studios::PureSG::InputPerturbations();
    auto option3 = new Studios::PureSG::InputGeneralOscillons();
    auto option4 = new Studios::PureSG::InputShockwave();
    auto option5 = new Studios::PureSG::InputSingleOscillon();

    /* sim 0 */selector.registerOption(option1->getInterface());
    /* sim 1 */selector.registerOption(option2->getInterface());
    /* sim 2 */selector.registerOption(option3->getInterface());
    /* sim 3 */selector.registerOption(option4->getInterface());
    /* sim 4 */selector.registerOption(option5->getInterface());

    auto selectedInterface = selector.preParse(argc, argv).getCurrentCandidate();
    auto input    = dynamic_cast<KGRtoRBuilder*>(selectedInterface->getOwner());
    auto inputPtr = KGRtoRBuilder_ptr(input);

    auto prog = App(argc, argv, inputPtr);

    return prog.run();
}

auto main(int argc, const char **argv) -> int {
    return Slab::Core::SafetyNet::jump(run, argc, argv);
}


