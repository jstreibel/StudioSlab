
#include "XYApp.h"

#include "App/CrashPad.h"


int run(int argc, const char *argv[]) {
    auto app = new XY::App(argc, argv);

    return app->run();
}


int main(int argc, const char *argv[]) {
    //return run(argc, argv);
    return Slab::Core::SafetyNet::jump(run, argc, argv);
}
