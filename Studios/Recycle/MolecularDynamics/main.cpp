#include "Simulation/Simulation.h"
#include "Hamiltonians/NewtonMechanicsParams.h"

int main(int /* argc */ , char** /* argv */ )
{

    try {
        BackendSFML output;

        Simulation sim(N_MOLS, &output);
        sim.run();
    } catch (const char *msg) {
        std::cout << "\n" << msg << "\n";
    }


    return 0;
}
