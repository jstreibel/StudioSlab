//
// Created by joao on 10/25/21.
//

#ifndef STUDIOSLAB_OMPUTILS_H
#define STUDIOSLAB_OMPUTILS_H

#include "Types.h"

// #define FORCE_NO_OMP

namespace Slab {

    IntPair getBeginAndEndForThreadedFor(int N);

#ifndef FORCE_NO_OMP

#define OMP_GET_BEGIN_END(beginVarName, endVarName, NSites) \
    IntPair be = getBeginAndEndForThreadedFor(NSites); \
    int beginVarName = be.first, endVarName = be.second;

/** Esse for espera estar em um ambiente (um escopo) onde globalmente seja valida
 * uma chamada a
 * #pragma omp parallel num_threads(NUM_THREADS) */
#define OMP_PARALLEL_FOR(indexLabel, NSites) \
    IntPair be = getBeginAndEndForThreadedFor(NSites); \
    for(UInt indexLabel=be.first; indexLabel<be.second; indexLabel++)

#else

#define OMP_GET_BEGIN_END(beginVarName, endVarName, NSites) \
    IntPair be(0, NSites);                                  \
    int beginVarName = be.first, endVarName = be.second;

#define OMP_PARALLEL_FOR(indexLabel, NSites) \
    for(UInt indexLabel=0; indexLabel<NSites; indexLabel++)

#endif // USE_OMP

}

#endif //STUDIOSLAB_OMPUTILS_H
