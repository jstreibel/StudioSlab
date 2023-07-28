//
// Created by joao on 10/8/21.
//

#include "Base/App/CrashPad.h"

#include "Base/Controller/Interface/InterfaceSelector.h"

#include "LeadingDelta/LeadingDelta.h"

#include "Mappings/R2toR/App/R2toR App.h"


int main(int argc, const char **argv) {

    try {
        auto input = new R2toR :: LeadingDelta :: Builder( );

        InterfaceManager::getInstance().registerInterface(input->getInterface());
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


