//
// Created by joao on 10/8/21.
//

#include "Base/App/CrashPad.h"

#include "Base/Controller/Interface/InterfaceSelector.h"

#include "GrowingHole/GrowingHoleInput.h"
#include "DiracSpeed/DiracSpeedInput.h"
#include "LeadingDelta/LeadingDelta.h"

#include "Mappings/R2toR/App/R2toR App.h"
#include "Common/Log/Log.h"


int main(int argc, const char **argv) {

    try {
        InterfaceSelector selector("Available simulations");

        auto option1 = new R2toR :: DiracSpeed   :: Builder( );
        auto option2 = new R2toR :: GrowingHole  :: Builder( );
        auto option3 = new R2toR :: LeadingDelta :: Builder( );

        /* sim 0 */ selector.registerOption( option1->getInterface() );
        /* sim 1 */ selector.registerOption( option2->getInterface() );
        /* sim 2 */ selector.registerOption( option3->getInterface() );

        auto selectedInterface = selector.preParse(argc, argv).getCurrentCandidate();
        auto input    = dynamic_cast<Base::Simulation::VoidBuilder*>(selectedInterface->getOwner());

        auto prog = R2toR::App::Simulations(argc, argv, Base::Simulation::VoidBuilder::Ptr(input));

        return SafetyNet::jump(prog);

    } catch (const char *e) {
        Log::Fatal() << "Exception reached the top of main: \"" << "\033[91m\033[1m"
                  << e << "\033[0m" << "\", application will now exit." << Log::Flush;
        return 1;
    } catch (const std::string e) {
        Log::Fatal() << "Exception reached the top of main: \"" << "\033[91m\033[1m"
                  << e << "\033[0m" << "\", application will now exit." << Log::Flush;
        return 2;
    } catch (cxxopts::exceptions::invalid_option_syntax e) {
        Log::Fatal() << e.what() << Log::Flush;
        return 3;
    }
    catch (cxxopts::exceptions::no_such_option &e) {
        Log::Fatal() << e.what() << Log::Flush;
        return 4;
    }
    catch (cxxopts::exceptions::incorrect_argument_type &e) {
        Log::Fatal() << e.what() << Log::Flush;
        return 5;
    }


}


