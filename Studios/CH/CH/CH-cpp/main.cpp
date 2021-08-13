#include "simulation.h"

int main(int /* argc */ , char** /* argv */ )
{
    Simulation mol(1024, 1.e-10, CHData::acc1);
    mol.run();

    return 0;
}
