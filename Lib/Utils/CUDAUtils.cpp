//
// Created by joao on 7/08/23.
//

#include "CUDAUtils.h"

#include "Types.h"

namespace Slab::CUDA {

#if USE_CUDA
    void cew(cudaError err) {

        auto errStr = cudaGetErrorString(err);

        auto errMsg = Str("CUDA error ") + Str(errStr);

        if (err != cudaError::cudaSuccess) throw Exception(errMsg);
    }

#endif

}