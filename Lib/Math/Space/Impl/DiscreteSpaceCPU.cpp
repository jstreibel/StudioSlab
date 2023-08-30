//

#include "DiscreteSpaceCPU.h"

#include <Utils/OMPUtils.h>

//
// Created by joao on 11/10/2019.
DiscreteSpaceCPU::DiscreteSpaceCPU(DimensionMetaData dim, Real h) : DiscreteSpace(dim, h) {

}

DiscreteSpace &DiscreteSpaceCPU::Add(const DiscreteSpace &func) {
    assert(getTotalDiscreteSites() == func.getTotalDiscreteSites()); // TODO: assert, tambem, que os intervalos sao os mesmos.

    auto &fVec = func.getHostData();
    auto N = getTotalDiscreteSites();

    OMP_PARALLEL_FOR(n, N)
        data[n] += fVec[n];

    return *this;
}

DiscreteSpace &DiscreteSpaceCPU::Subtract(const DiscreteSpace &func) {
    assert(getTotalDiscreteSites() == func.getTotalDiscreteSites()); // TODO: assert, tambem, que os intervalos sao os mesmos.

    auto &fVec = func.getHostData();
    auto N = getTotalDiscreteSites();

    OMP_PARALLEL_FOR(n, N)
        data[n] -= fVec[n];

    return *this;}

DiscreteSpace &DiscreteSpaceCPU::StoreAddition(const DiscreteSpace &space1, const DiscreteSpace &space2) {
    assert(getTotalDiscreteSites() == space1.getTotalDiscreteSites() && getTotalDiscreteSites() == space2.getTotalDiscreteSites()); // TODO: assert, tambem, que os intervalos sao os mesmos.

    auto &f1Vec = space1.getHostData(), &f2Vec = space2.getHostData();
    auto N = getTotalDiscreteSites();

    OMP_PARALLEL_FOR(n, N)
        data[n] = f1Vec[n] + f2Vec[n];

    return *this;
}

DiscreteSpace &DiscreteSpaceCPU::StoreSubtraction(const DiscreteSpace &space1, const DiscreteSpace &space2) {
    assert(getTotalDiscreteSites() == space1.getTotalDiscreteSites() && getTotalDiscreteSites() == space2.getTotalDiscreteSites()); // TODO: assert, tambem, que os intervalos sao os mesmos.

    auto &f1Vec = space1.getHostData(), &f2Vec = space2.getHostData();
    auto N = getTotalDiscreteSites();

    OMP_PARALLEL_FOR(n, N)
        data[n] = f1Vec[n] - f2Vec[n];

    return *this;
}

DiscreteSpace &DiscreteSpaceCPU::Multiply(floatt a) {
    auto N = getTotalDiscreteSites();

    OMP_PARALLEL_FOR(n, N)
        data[n] *= a;

    return *this;
}

void DiscreteSpaceCPU::setToValue(const DiscreteSpace &inSpace) {
    RealVector &X = getHostData();
    const RealVector &fVec = inSpace.getHostData();
    const PosInt N = inSpace.getTotalDiscreteSites();

    OMP_PARALLEL_FOR(n, N)
        X[n] = fVec[n];
}

DiscreteSpace &DiscreteSpaceCPU::StoreMultiplication(const DiscreteSpace &space1, const Real a) {
    assert(getTotalDiscreteSites() == space1.getTotalDiscreteSites()); // TODO: assert, tambem, que os intervalos sao os mesmos.

    auto &f1Vec = space1.getHostData();
    auto N = getTotalDiscreteSites();

    OMP_PARALLEL_FOR(n, N)
        data[n] = f1Vec[n] * a;

    return *this;
}
