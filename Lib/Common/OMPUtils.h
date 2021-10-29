//
// Created by joao on 10/25/21.
//

#ifndef STUDIOSLAB_OMPUTILS_H
#define STUDIOSLAB_OMPUTILS_H

#include "Typedefs.h"

IntPair getBeginAndEndForThreadedFor(int N);

#define OMP_GET_BEGIN_END(beginVarName, endVarName, NSites) \
    IntPair be = getBeginAndEndForThreadedFor(NSites); \
    int beginVarName = be.first, endVarName = be.second;

/** Esse for espera estar em um ambiente (um escopo) onde globalmente seja valida
 * uma chamada a
 * #pragma omp parallel num_threads(NUM_THREADS) */
#define OMP_PARALLEL_FOR(indexLabel, NSites) \
    IntPair be = getBeginAndEndForThreadedFor(NSites); \
    for(PosInt indexLabel=be.first; indexLabel<be.second; indexLabel++)

#endif //STUDIOSLAB_OMPUTILS_H
