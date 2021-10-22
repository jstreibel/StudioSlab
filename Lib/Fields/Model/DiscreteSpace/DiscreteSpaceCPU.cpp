//

#include "DiscreteSpaceCPU.h"

//
// Created by joao on 11/10/2019.
DiscreteSpaceCPU::DiscreteSpaceCPU(DimensionMetaData dim, Real h) : DiscreteSpace(dim, h) {

}

DiscreteSpace &DiscreteSpaceCPU::Add(const DiscreteSpace &func) {
    assert(getTotalDiscreteSites() == func.getTotalDiscreteSites()); // TODO: assert, tambem, que os intervalos sao os mesmos.

    auto &fVec = func.getX();
    auto N = getTotalDiscreteSites();

    OMP_PARALLEL_FOR(n, N)
        X[n] += fVec[n];

    return *this;
}

DiscreteSpace &DiscreteSpaceCPU::StoreAddition(const DiscreteSpace &space1, const DiscreteSpace &space2) {
    assert(getTotalDiscreteSites() == space1.getTotalDiscreteSites() && getTotalDiscreteSites() == space2.getTotalDiscreteSites()); // TODO: assert, tambem, que os intervalos sao os mesmos.

    auto &f1Vec = space1.getX(), &f2Vec = space2.getX();
    auto N = getTotalDiscreteSites();

    OMP_PARALLEL_FOR(n, N)
        X[n] = f1Vec[n] + f2Vec[n];

    return *this;
}

DiscreteSpace &DiscreteSpaceCPU::StoreSubtraction(const DiscreteSpace &space1, const DiscreteSpace &space2) {
    assert(getTotalDiscreteSites() == space1.getTotalDiscreteSites() && getTotalDiscreteSites() == space2.getTotalDiscreteSites()); // TODO: assert, tambem, que os intervalos sao os mesmos.

    auto &f1Vec = space1.getX(), &f2Vec = space2.getX();
    auto N = getTotalDiscreteSites();

    OMP_PARALLEL_FOR(n, N)
        X[n] = f1Vec[n] - f2Vec[n];

    return *this;
}

DiscreteSpace &DiscreteSpaceCPU::Multiply(floatt a) {
    auto N = getTotalDiscreteSites();

    OMP_PARALLEL_FOR(n, N)
        X[n] *= a;

    return *this;
}

void DiscreteSpaceCPU::setToValue(const DiscreteSpace &inSpace) {
    VecFloat &X = getX();
    const VecFloat &fVec = inSpace.getX();
    const PosInt N = inSpace.getTotalDiscreteSites();

    OMP_PARALLEL_FOR(n, N)
        X[n] = fVec[n];
}
