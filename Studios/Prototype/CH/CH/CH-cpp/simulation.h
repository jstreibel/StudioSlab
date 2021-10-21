#ifndef MOLSIMULATION_H
#define MOLSIMULATION_H

#include "CahnHilliard.h"
#include "defs.h"

class Simulation
{
public:
    Simulation(size_t N, const double h, const CHData::DConfig dconfig);
    void run();

    //MoleculContainer molecules;
    CHData phi;
};

#endif // MOLSIMULATION_H
