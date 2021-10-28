//
// Created by joao on 29/08/2021.
//

#include "SafetyNet.h"

#include "../Util/STDLibInclude.h"

#include <boost/program_options.hpp>


void showHelp(AppBase &prog){
    //prog.buildFullCommandLineOptions().print(std::cout);
}


int SafetyNet::jump(int argc, const char *argv[], AppBase &prog){
    // if (GPU_DEBUG) std::cout << "\033[1m\033[93mGPU IS IN DEBUG MODE => NO GPU.\033[0m" << std::endl;

    try {
        return prog.run();
    }
    catch (const char *e) {
        std::cout << "\n\nUnhandled Exception reached the top of main: " << "\033[91m\033[1m"
        << e << "\033[0m" << ", application will now exit" << std::endl;
        return -1;
    }
    catch (String &e) {
        std::cout << "\n\nUnhandled Exception reached the top of main: " << "\033[91m\033[1m"
        << e << "\033[0m" << ", application will now exit" << std::endl;
        return -2;
    }
    catch (boost::program_options::error &e) {
        showHelp(prog);
        std::cout << "Error parsing command line: \"" << e.what() << "\" " << "Args are";
        for (int i = 0; i < argc; i++)
            std::cout << "\n\t" << argv[i];
        std::cout << std::endl;

        return -3;
    }
    catch (boost::bad_any_cast &e) {
        showHelp(prog);
        std::cout << "\n\nUnhandled boost::bad_any_cast reached the top of main: " << "\033[91m\033[1m"
        << e.what() << "\033[0m. \n" << std::endl;

        return -4;
    }
    catch (std::bad_cast &e) {
        showHelp(prog);
        std::cout << "\n\nUnhandled std::bad_cast reached the top of main: " << "\033[91m\033[1m"
        << e.what() << "\033[0m. \n" << std::endl;
        return -5;
    }
    catch (std::exception &e) {
        std::cout << "\n\nUnhandled std::exception reached the top of main: " << "\033[91m\033[1m"
        << e.what() << "\033[0m, application will now exit. \n" << std::endl;
        return -6;
    }
    catch (...) {
        std::cout << "\n\nUnknown exception reached the top of main. \n" << std::endl;
    }

    throw "Dafuk...";
}