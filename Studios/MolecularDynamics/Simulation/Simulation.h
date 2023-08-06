#ifndef MOLSIMULATION_H
#define MOLSIMULATION_H


#include "Hamiltonians/Lennard-Jones/LennardJones.h"
#include "Hamiltonians/SoftDisk/SoftDisk.h"
#include "Hamiltonians/SoftDisk/Langevin.h"

#include "IO/KeyPressListener.h"
#include "IO/BackendSFML.h"

#include "Hamiltonians/Types.h"


class Simulation : public KeyPressListener
{
public:
    Simulation(size_t N, BackendSFML *output);
    void run();


    void KeyPressed(sf::Event::KeyEvent &keyEvent, int value) override;


private:
    BackendSFML *output;


    LennardJones physModelMolDynamic;
    PointContainer q, p;


    bool isFinished;

};

#endif // MOLSIMULATION_H
