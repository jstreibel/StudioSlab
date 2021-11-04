
#include "XYApp.h"

#include "../../xy/BasicSim.h"

#include <Studios/App/CrashPad.h>

int main(int argc, const char *argv[]) {

    if(1){
        auto app = new XYApp(argc, argv);

        SafetyNet::jump(*app);

        return 0;
    }

    try {
        return RunBasicSim(argc, argv);
    } catch (const char *msg){
        std::cout << "Exception: \"" << msg << "\". Terminating";
    }
}
