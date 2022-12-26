//

#include "DiscreteSpace.h"

#include "DiscreteSpaceCPU.h"
#include <assert.h>

// Created by joao on 11/10/2019.

//
PosInt DimensionMetaData::computeFullDiscreteSize() const {
    PosInt size = 1;
    for(auto n : N){
        size *= n;
    }

    return size;
}

DiscreteSpace::DiscreteSpace(DimensionMetaData dim, Real h) : dim(dim), X(dim.computeFullDiscreteSize()), h(h) {

}

DiscreteSpaceCPU *DiscreteSpace::hostCopy(PosInt maxResolution) const {
    PosInt maxDim = 0;
    for(auto N : dim.getN()) maxDim = std::max(maxDim, N);
    Real r = Real(maxResolution) / Real(maxDim);

    std::vector<PosInt> N(dim.getNDim());

    for(PosInt i=0; i<dim.getNDim(); i++) N[i] = r * dim[i];
    DimensionMetaData newDim(N);
    DiscreteSpaceCPU *theCopy = new DiscreteSpaceCPU(newDim, h/r);

    const VecFloat& X = getX();
    VecFloat &reducedX = theCopy->getX();
    if(newDim.getNDim() == 1)
        for(PosInt i = 0; i<dim[0]; i++)
            reducedX[r*i] = X[i];
    else if (newDim.getNDim() == 2)
        for(PosInt i=0; i<N[0]; i++)
            for (PosInt j = 0; j < N[1]; j++) {
                const PosInt index = i+j*N[0];
                const PosInt indexOriginal = r*i + r*j*dim[0]; // Maybe not good?
                reducedX[index] = X[indexOriginal];
            }
    else if (newDim.getNDim() == 3){
        // TODO idealmente, aqui deveria ocorrer recursao pra garantir o algoritmo para qualquer numero de dimensoes.
        throw "3D DiscreteSpace::hostCopy not implemented.";
    }

    return theCopy;
}

PosInt DiscreteSpace::getTotalDiscreteSites() const {
    assert(X.size() == dim.computeFullDiscreteSize());
    return X.size();
}

const VecFloat &DiscreteSpace::getHostData(bool sync) const {
    if(sync) syncHost();

    return X;
}

VecFloat &DiscreteSpace::getHostData(bool syncWithServer) {
    if(syncWithServer) syncHost();

    return X;
}

void DiscreteSpace::syncHost() const { }
void DiscreteSpace::upload() { }

const VecFloat &DiscreteSpace::getX() const {
    return getHostData(true);
}

VecFloat &DiscreteSpace::getX() {
    syncHost();

    return X;
}

Real DiscreteSpace::geth() const {
    return h;
}

