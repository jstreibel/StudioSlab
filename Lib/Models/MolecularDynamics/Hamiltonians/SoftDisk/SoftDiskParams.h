//
// Created by joao on 05/08/2021.
//

#ifndef MOLEKUL_PLAY_SOFTDISKPARAMS_H
#define MOLEKUL_PLAY_SOFTDISKPARAMS_H

#include "../Particle.h"

#include "Utils/Types.h"

namespace Slab::Models::MolecularDynamics {

// PARA FORMAR CLUSTERS:
//#define N_MOLS 300
//#define ε 1.0
//#define σ 0.5 // raio da interacao
//#define ALPHA 5.0 // expoente do potencial
//#define SIMSPACE_SIDE_SIZE 2.00 // metade da largura do espaco


#define ε 1.0
#define σ CUTOFF_RADIUS // raio da interacao
#define ALPHA 2 // expoente do potencial (quanto maior, mais soft)

    const DevFloat Temperature = 10e-3;

}

#endif //MOLEKUL_PLAY_SOFTDISKPARAMS_H
