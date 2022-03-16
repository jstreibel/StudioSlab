

#include "BasicSim.h"
#include "ManyHistories.h"
#include "ManyHistoriesManyTemperatures.h"


int main(int argc, char *argv[]) {

    try {
        if (true) RunManyHistories(argc, argv);
        else if(true) RunManyHistoriesManyTemperatures(argc, argv);
        else return RunBasicSim(argc, argv);
    } catch (const char *msg){
        std::cout << "Exception: \"" << msg << "\". Terminating";
    }
}
