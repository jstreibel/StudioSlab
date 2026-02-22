//
// Created by joao on 10/25/21.
//

#include "OMPUtils.h"

#if OMP_SUPPORT == true
#include <omp.h>
#endif


namespace Slab {

    IntPair GetBeginAndEndForThreadedFor(int N) {
#if OMP_SUPPORT == true
        const int nThreads = omp_get_num_threads();
        const int myThreadRank = omp_get_thread_num();
        const int block_size = N / nThreads;
        const int begin = myThreadRank * block_size;
        const int end = begin + block_size;
        assert(N % nThreads == 0);
#else
        const int begin = 0;
        const int end = N;
#endif

        return {begin, end};
    }


}
