//

#include "DiscreteSpace.h"

#include "DiscreteSpaceCPU.h"
#include <assert.h>

// Created by joao on 11/10/2019.

//
UInt DimensionMetaData::computeFullDiscreteSize() const {
    UInt size = 1;
    for(auto n : N){
        size *= n;
    }

    return size;
}

DiscreteSpace::DiscreteSpace(DimensionMetaData dim) : dimensionMetaData(dim), data(dim.computeFullDiscreteSize()) {

}

DiscreteSpace::~DiscreteSpace() { }

DiscreteSpaceCPU *DiscreteSpace::hostCopy(UInt maxResolution) const {
    auto &dim = dimensionMetaData;

    UInt maxDim = 0;
    for(auto N : dim.getN()) maxDim = std::max(maxDim, N);
    Real r = Real(maxResolution) / Real(maxDim);

    UIntArray N(dim.getNDim());
    RealArray h(dim.getNDim());


    for(UInt i=0; i<dim.getNDim(); i++){
        N[i] = UInt(r * dim.getN(i));;
        h[i] = dim.geth(i)/r;;
    }

    DimensionMetaData newDim(N, h);
    auto *theCopy = new DiscreteSpaceCPU(newDim);

    const RealArray& X = getHostData();
    RealArray &reducedX = theCopy->getHostData();
    if(newDim.getNDim() == 1)
        for(UInt i = 0; i<dim.getN(0); i++)
            reducedX[UInt(r*i)] = X[i];     // TODO lento bagaray, ridiculamente simples deixar mais rapido vamos combinar
    else if (newDim.getNDim() == 2)
        for(UInt i=0; i<N[0]; i++)
            for (UInt j = 0; j < N[1]; j++) {
                const UInt index = i+j*N[0];
                const UInt indexOriginal = r*i + r*j*dim.getN(0); // Maybe not good?
                reducedX[index] = X[indexOriginal];
            }
    else if (newDim.getNDim() == 3){
        // TODO idealmente, aqui deveria ocorrer recursao pra garantir o algoritmo para qualquer numero de dimensoes.
        throw "3D DiscreteSpace::hostCopy not implemented.";
    }

    return theCopy;
}

UInt DiscreteSpace::getTotalDiscreteSites() const {
    assert(data.size() == dim.computeFullDiscreteSize());
    return data.size();
}


void DiscreteSpace::syncHost() const { }
void DiscreteSpace::upload() { }

const RealArray &DiscreteSpace::getHostData(bool sync) const { if(sync) syncHost(); return data; }
RealArray       &DiscreteSpace::getHostData(bool sync)       { if(sync) syncHost(); return data; }



