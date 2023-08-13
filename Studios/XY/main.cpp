
#include "XYApp.h"

#include "Base/App/CrashPad.h"


int run(int argc, const char *argv[]) {
    auto app = new XY::App(argc, argv);

    return app->run();
}


int main(int argc, const char *argv[]) {
    //return run(argc, argv);
    return SafetyNet::jump(run, argc, argv);
}
