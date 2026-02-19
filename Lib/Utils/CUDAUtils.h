//
// Created by joao on 7/08/23.
//

#ifndef STUDIOSLAB_CUDAUTILS_H
#define STUDIOSLAB_CUDAUTILS_H

#if USE_CUDA

#include <cuda_runtime.h>

namespace Slab::CUDA {

    void CheckCudaError(cudaError err);

    [[deprecated("Use CheckCudaError")]]
    void cew(cudaError err);

}

#endif // USE_CUDA

#endif //STUDIOSLAB_CUDAUTILS_H
