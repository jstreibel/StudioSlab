//
// Created by joao on 29/08/2021.
//

#include "CrashPad.h"
#include "Common/Log/Log.h"

#include <Common/STDLibInclude.h>
#include <Base/Controller/CLArgsManager.h>

#include <cxxopts.hpp>


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
        Log::ErrorFatal() << "Exception: " << "\033[91m\033[1m"
        << e << "\033[0m" << ", application will now exit" << Log::Flush;
        return -1;
    }
    catch (Str &e) {
        Log::ErrorFatal() << "Exception: " << "\033[91m\033[1m"
        << e << "\033[0m" << ", application will now exit" << Log::Flush;
        return -2;
    }
    catch (cxxopts::exceptions::invalid_option_syntax e) {
        Log::ErrorFatal() << "Invalid option syntax: \"\\033[91m\\033[1m" << e.what()
                          << "\033[0m\"." << Log::Flush;
        return -3;
    }
    catch (cxxopts::exceptions::no_such_option &e) {
        Log::ErrorFatal() << "No such option: \"\\033[91m\\033[1m" << e.what()
                          << "\033[0m\"." << Log::Flush;
        return -4;
    }
    catch (cxxopts::exceptions::incorrect_argument_type &e) {
        Log::ErrorFatal() << "Incorrect argument type: \"\\033[91m\\033[1m" << e.what()
                          << "\033[0m\"." << Log::Flush;
        return -5;
    }
    catch (cxxopts::exceptions::exception &e) {
        showHelp(prog);
        Log::ErrorFatal() << "Error parsing command line: \"" << e.what() << "\" " << Log::Flush;

        return -6;
    }
    catch (std::bad_cast &e) {
        showHelp(prog);
        Log::ErrorFatal() << "Exception std::bad_cast: " << "\033[91m\033[1m"
        << e.what() << "\033[0m." << Log::Flush;

        return -7;
    }
    catch (std::exception &e) {
        Log::ErrorFatal() << "Exception std::exceptionn: " << "\033[91m\033[1m"
        << e.what() << "\033[0m, application will now exit." << Log::Flush;
        return -8;
    }
    catch (...) {
        Log::ErrorFatal() << "Unknown exception reached the top of main." << Log::Flush;
        return -9;
    }

    throw "Dafuk...";
}