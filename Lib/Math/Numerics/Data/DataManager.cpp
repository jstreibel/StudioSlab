//
// Created by joao on 26/05/24.
//

#include "DataManager.h"
#include "Math/Function/R2toR/Model/R2toRDiscreteFunctionCPU.h"

namespace Slab::Math {

    DataManager::DataManager() : Singleton("Data Manager") {}

        R2toR::DiscreteFunction_ptr DataManager::NewFunctionR2toRDDataSet(Str uniqueName, Resolution N, Resolution M, Real2D rMin, Real2D r) {
        fix hx = r.x/(Real)N;
        fix hy = r.y/(Real)M;

        auto data = Slab::New<R2toR::DiscreteFunction_CPU>(N, M, rMin.x, rMin.y, hx, hy);
        auto dataSet = New<DataSet>(data, "R2toR::DiscreteFunction");

        NOT_IMPLEMENTED

    }
} // Slab::Math