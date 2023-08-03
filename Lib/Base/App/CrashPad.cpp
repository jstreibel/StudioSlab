//
// Created by joao on 29/08/2021.
//

#include "CrashPad.h"
#include "Common/Log/Log.h"

#include <Common/STDLibInclude.h>
#include <Base/Controller/CLArgsManager.h>

#include <cxxopts.hpp>


void none() {};

void showHelp()
{
    if(true) none();
    else CLArgsManager::ShowHelp();
}


AppBase *prog = nullptr;

int runProg(int, const char**) {
    return prog->run();
}

Str FORMAT;

#define LogException(description, what, exitFunc)                                       \
    { Log::Fatal() << description << ": \"" << FORMAT << what << Log::ResetFormatting   \
                   << "\". Application will now exit." << Log::Flush;                    \
      exitFunc();                                                                       \
      return EXIT_FAILURE; }

int SafetyNet::jump(int (*pFunction)(int argc, const char **argv), int argc, const char *argv[]) {
    try {
        FORMAT = Log::BGRed + Log::BoldFace;
        return pFunction(argc, argv);
    }
    catch (const char *e)                                   LogException("Exception (const char*)",  e,        none)
    catch (Str &e)                                          LogException("Exception (std::string)",  e,        none)
    catch (cxxopts::exceptions::invalid_option_syntax e)    LogException("Invalid option syntax",    e.what(), showHelp)
    catch (cxxopts::exceptions::no_such_option &e)          LogException("No such option",           e.what(), showHelp)
    catch (cxxopts::exceptions::option_already_exists &e)   LogException("Option already exists",    e.what(), showHelp)
    catch (cxxopts::exceptions::incorrect_argument_type &e) LogException("Incorrect argument type",  e.what(), showHelp)
    catch (std::bad_cast &e)                                LogException("Bad cast",                 e.what(), none)
    catch (cxxopts::exceptions::exception &e)               LogException("Parsing exception",        e.what(), none)
    catch (std::exception &e)                               LogException("Exception std::exception", e.what(), none)
    catch (...)                                             LogException("Unknown exception",        "...",    none)

    throw "Impossible.";
}

