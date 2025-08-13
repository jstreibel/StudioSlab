

#include "CrashPad.h"
#include "Core/Controller/CommandLine/CommandLineInterfaceSelector.h"

#include "MathApp.h"

#include "InteractingFormations/input-sym-oscillons.h"
#include "InteractingFormations/input-general-oscillons.h"
#include "SingleFormations/input-perturbed.h"
#include "SingleFormations/input-shockwave.h"
#include "SingleFormations/InputSingleOscillon.h"

int run(int argc, const char **argv) {
    using namespace Slab::Core;
    using namespace Slab::Math;
    using namespace Slab::Models::KGRtoR;

    CLInterfaceSelector selector("Simulation builder selector");

    auto option0 = new Studios::PureSG::InputSymmetricOscillon();
    auto option1 = new Studios::PureSG::InputPerturbations();
    auto option2 = new Studios::PureSG::InputGeneralOscillons();
    auto option3 = new Studios::PureSG::InputShockwave();
    auto option4 = new Studios::PureSG::InputSingleOscillon();

    /* sim 0 */selector.registerOption(option0->GetInterface());
    /* sim 1 */selector.registerOption(option1->GetInterface());
    /* sim 2 */selector.registerOption(option2->GetInterface());
    /* sim 3 */selector.registerOption(option3->GetInterface());
    /* sim 4 */selector.registerOption(option4->GetInterface());

    auto selectedInterface = selector.preParse(argc, argv).getCurrentCandidate();
    auto input    = dynamic_cast<FKGRtoR_Recipe*>(selectedInterface->GetOwner());
    auto inputPtr = KGRtoRBuilder_ptr(input);

    auto prog = MathApp(argc, argv, inputPtr);

    return prog.run();
}

auto main(int argc, const char **argv) -> int {
    return Slab::SafetyNet::jump(run, argc, argv);
}


