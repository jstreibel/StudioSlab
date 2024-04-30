//
// Created by joao on 10/25/21.
//

#include "OMPUtils.h"
#include "omp.h"

IntPair getBeginAndEndForThreadedFor(int N) {
    const int nThreads = omp_get_num_threads();
    const int myThreadRank = omp_get_thread_num();
    const int block_size = N/nThreads;
    const int begin = myThreadRank * block_size;
    const int end = begin + block_size;

    assert(N%nThreads == 0);

    return {begin, end};
}