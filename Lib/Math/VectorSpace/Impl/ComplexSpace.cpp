//
// Created by joao on 10/05/24.
//

#include "ComplexSpace.h"

#include <Utils/OMPUtils.h>


namespace Slab::Math {

    DiscreteComplexSpace::DiscreteComplexSpace(const DimensionMetaData& dim)
            : dimensionMetaData(dim), data(dim.computeFullDiscreteSize())
    { }

    UInt DiscreteComplexSpace::getTotalDiscreteSites() const {
        assert(data.size() == dimensionMetaData.computeFullDiscreteSize());
        return data.size();
    }

    DiscreteComplexSpace &DiscreteComplexSpace::Add(const DiscreteComplexSpace &func) {
        assert(getTotalDiscreteSites() == func.getTotalDiscreteSites()); // TODO: assert, tambem, que os intervalos sao os mesmos.

        auto &fVec = func.data;
        auto N = getTotalDiscreteSites();

        OMP_PARALLEL_FOR(n, N)
        data[n] += fVec[n];

        return *this;
    }

    DiscreteComplexSpace &DiscreteComplexSpace::Subtract(const DiscreteComplexSpace &func) {
        assert(getTotalDiscreteSites() == func.getTotalDiscreteSites()); // TODO: assert, tambem, que os intervalos sao os mesmos.

        auto &fVec = func.data;
        auto N = getTotalDiscreteSites();

        OMP_PARALLEL_FOR(n, N)
        data[n] -= fVec[n];

        return *this;}

    DiscreteComplexSpace &DiscreteComplexSpace::StoreAddition(const DiscreteComplexSpace &space1, const DiscreteComplexSpace &space2) {
        assert(getTotalDiscreteSites() == space1.getTotalDiscreteSites() && getTotalDiscreteSites() == space2.getTotalDiscreteSites()); // TODO: assert, tambem, que os intervalos sao os mesmos.

        auto &f1Vec = space1.data, &f2Vec = space2.data;
        auto N = getTotalDiscreteSites();

        OMP_PARALLEL_FOR(n, N)
        data[n] = f1Vec[n] + f2Vec[n];

        return *this;
    }

    DiscreteComplexSpace &DiscreteComplexSpace::StoreSubtraction(const DiscreteComplexSpace &space1, const DiscreteComplexSpace &space2) {
        assert(getTotalDiscreteSites() == space1.getTotalDiscreteSites() && getTotalDiscreteSites() == space2.getTotalDiscreteSites()); // TODO: assert, tambem, que os intervalos sao os mesmos.

        auto &f1Vec = space1.data, &f2Vec = space2.data;
        auto N = getTotalDiscreteSites();

        OMP_PARALLEL_FOR(n, N)
        data[n] = f1Vec[n] - f2Vec[n];

        return *this;
    }

    DiscreteComplexSpace &DiscreteComplexSpace::Multiply(floatt a) {
        auto N = getTotalDiscreteSites();

        OMP_PARALLEL_FOR(n, N)
        data[n] *= a;

        return *this;
    }

    void DiscreteComplexSpace::setToValue(const DiscreteComplexSpace &inSpace) {
        auto &X = data;
        const auto &fVec = inSpace.data;
        const UInt N = inSpace.getTotalDiscreteSites();

        OMP_PARALLEL_FOR(n, N)
        X[n] = fVec[n];
    }

    DiscreteComplexSpace &DiscreteComplexSpace::StoreScalarMultiplication(const DiscreteComplexSpace &space1, const Real a) {
        assert(getTotalDiscreteSites() == space1.getTotalDiscreteSites()); // TODO: assert, tambem, que os intervalos sao os mesmos.

        auto &f1Vec = space1.data;
        auto N = getTotalDiscreteSites();

        OMP_PARALLEL_FOR(n, N)
        data[n] = f1Vec[n] * a;

        return *this;
    }

    auto DiscreteComplexSpace::getData() const -> const ComplexArray & {
        return data;
    }

    auto DiscreteComplexSpace::getData() -> ComplexArray & {
        return data;
    }

    DiscreteComplexSpace &DiscreteComplexSpace::operator*=(Real a) {
        return NumericAlgebra::operator*=(a);
    }

} // Math