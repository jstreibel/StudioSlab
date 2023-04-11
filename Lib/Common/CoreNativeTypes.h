//
// Created by joao on 10/25/21.
//

#ifndef STUDIOSLAB_NATIVEREALNUMBER_H
#define STUDIOSLAB_NATIVEREALNUMBER_H

#include <memory>
#include <cstdint>
#include <assert.h>

#ifndef __CUDACC__ // se estiver sendo compilado pelo nvcc

#endif

#define FLOAT64 double
#define FLOAT32 float

typedef FLOAT64 floatt;

typedef floatt Real;

typedef std::size_t PosInt;
typedef int_least64_t BigInt;

#include <vector>
typedef std::vector<Real> VecFloat;
typedef const VecFloat VecFloat_I;
typedef VecFloat VecFloat_O;
typedef VecFloat VecFloat_IO;

#endif //STUDIOSLAB_NATIVEREALNUMBER_H
