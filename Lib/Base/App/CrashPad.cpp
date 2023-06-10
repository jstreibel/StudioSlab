//
// Created by joao on 29/08/2021.
//

#include "CrashPad.h"
#include "Common/Log/Log.h"

#include <Common/STDLibInclude.h>
#include <Base/Controller/CLArgsManager.h>

#include <boost/program_options.hpp>


void showHelp(AppBase &prog)
{
    CLArgsManager::ShowHelp();
}


int SafetyNet::jump(AppBase &prog){
    // if (GPU_DEBUG) std::cout << "\033[1m\033[93mGPU IS IN DEBUG MODE => NO GPU.\033[0m" << std::endl;

    try {
        return prog.run();
    }
    catch (const char *e) {
        Log::ErrorFatal() << "\n\nUnhandled Exception reached the top of main: " << "\033[91m\033[1m"
        << e << "\033[0m" << ", application will now exit" << Log::Flush;
        return -1;
    }
    catch (String &e) {
        Log::ErrorFatal() << "\n\nUnhandled Exception reached the top of main: " << "\033[91m\033[1m"
        << e << "\033[0m" << ", application will now exit" << Log::Flush;
        return -2;
    }
    catch (boost::wrapexcept<boost::program_options::unknown_option> &e) {
        Log::ErrorFatal() << "Option \"\\033[91m\\033[1m\"\"" << e.get_option_name() << "\" error: " << e.what()
                  << "\033[0m. \n" << Log::Flush;
    }
    catch (boost::program_options::error &e) {
        showHelp(prog);
        Log::ErrorFatal() << "Error parsing command line: \"" << e.what() << "\" " << Log::Flush;

        return -3;
    }
    catch (boost::bad_any_cast &e) {
        showHelp(prog);
        Log::ErrorFatal() << "\n\nUnhandled boost::bad_any_cast reached the top of main: " << "\033[91m\033[1m"
        << e.what() << "\033[0m. \n" << Log::Flush;

        return -4;
    }
    catch (std::bad_cast &e) {
        showHelp(prog);
        Log::ErrorFatal() << "\n\nUnhandled std::bad_cast reached the top of main: " << "\033[91m\033[1m"
        << e.what() << "\033[0m. \n" << Log::Flush;
        return -5;
    }
    catch (std::exception &e) {
        Log::ErrorFatal() << "\n\nUnhandled std::exception reached the top of main: " << "\033[91m\033[1m"
        << e.what() << "\033[0m, application will now exit. \n" << Log::Flush;
        return -6;
    }
    catch (...) {
        Log::ErrorFatal() << "\n\nUnknown exception reached the top of main. \n" << Log::Flush;
        return -7;
    }

    throw "Dafuk...";
}