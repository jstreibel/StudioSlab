//
// Created by joao on 29/08/2021.
//

#include "CrashPad.h"
#include "Core/Tools/Log.h"

#include <Utils/STDLibInclude.h>
#include <Core/Controller/CLArgsManager.h>

#include <cxxopts.hpp>

#include <boost/stacktrace.hpp>
#include <iostream>
#include <stdexcept>

#define WITH_STACK_TRACE

#ifdef NDEBUG
#define RELEASE_COMPILE
#endif


namespace Slab::Core::SafetyNet {

    void none() {
#ifndef RELEASE_COMPILE
        Log::ErrorFatal() << "Stacktrace:\n" << boost::stacktrace::stacktrace() << Log::Flush;
#endif
    }

    void showHelp()
    {
        if(true) none();
        else CLArgsManager::ShowHelp();
    }

    Str FORMAT;

#define LogException(description, what, exitFunc)                                       \
    { Log::ErrorFatal() << description << ": " << FORMAT << " " << what << " " << Log::ResetFormatting   \
                   << ". Application will now exit." << Log::Flush;                    \
      exitFunc();                                                                       \
      return EXIT_FAILURE; }

    int jump(int (*pFunction)(int argc, const char **argv), int argc, const char *argv[]) {
#if defined(RELEASE_COMPILE) || defined(WITH_STACK_TRACE)
        try
#endif
        {
            FORMAT = Log::BGRed + Log::BoldFace + Log::FGBlack;
            return pFunction(argc, argv);
        }
#if defined(RELEASE_COMPILE) || defined(WITH_STACK_TRACE)
        catch (const char *e)                                   LogException("Exception (const char*)",  e,        none)
        catch (Str &e)                                          LogException("Exception (std::string)",  e,        none)
        catch (cxxopts::exceptions::invalid_option_syntax &e)   LogException("Invalid option syntax",    e.what(), showHelp)
        catch (cxxopts::exceptions::no_such_option &e)          LogException("No such option",           e.what(), showHelp)
        catch (cxxopts::exceptions::option_already_exists &e)   LogException("Option already exists",    e.what(), showHelp)
        catch (cxxopts::exceptions::incorrect_argument_type &e) LogException("Incorrect argument type",  e.what(), showHelp)
        catch (std::bad_cast &e)                                LogException("Bad cast",                 e.what(), none)
        catch (cxxopts::exceptions::exception &e)               LogException("Parsing exception",        e.what(), none)
        catch (std::exception &e)                               LogException("Exception std::exception", e.what(), none)
        catch (...)                                             LogException("Unknown exception",        "...",    none)
#endif
    }

}