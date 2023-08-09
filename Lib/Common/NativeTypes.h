//
// Created by joao on 10/25/21.
//

#ifndef STUDIOSLAB_NATIVEREALNUMBER_H
#define STUDIOSLAB_NATIVEREALNUMBER_H

#include <memory>
#include <cstdint>
#include <assert.h>

#ifndef __CUDACC__

#endif

#define FLOAT64 double
#define FLOAT32 float

#if STUDIOSLAB_USE_FP32
typedef FLOAT32 floatt;
#ifdef __CUDACC__
#pragma message "Using FP32 (nvcc)"
#endif
#else
typedef FLOAT64 floatt;
#ifdef __CUDACC__
// #pragma message "Using FP64 (nvcc)"
#endif
#endif


#endif //STUDIOSLAB_NATIVEREALNUMBER_H
