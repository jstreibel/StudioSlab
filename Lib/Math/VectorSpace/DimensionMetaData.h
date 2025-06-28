//
// Created by joao on 13/05/24.
//

#ifndef STUDIOSLAB_DIMENSIONMETADATA_H
#define STUDIOSLAB_DIMENSIONMETADATA_H

#include "Utils/Arrays.h"


namespace Slab::Math {

    struct DimensionMetaData {
        /*!
         * Constructor.
         * @param N A vector of size=dim and the discrete size of each dimension in
         * each of its entry. For example, a discretization of real space x, y, z
         * with 256x512x64 sites would have N={256,512,64}.
         */
        explicit DimensionMetaData(const UIntArray &N, RealArray _h) : nDim(N.size()), N(N), h(std::move(_h)) {
            assert(N.size() == h.size());
        }

        UInt getNDim() const { return nDim; }

        auto getN(UInt dim) const -> const UInt & { return N[dim]; }

        auto getN() const -> const UIntArray & { return N; }

        auto geth(UInt dim) const -> const DevFloat & { return h[dim]; }

        auto geth() const -> const RealArray & { return h; }

        auto getL(UInt dim) const -> DevFloat { return N[dim] * h[dim]; }

        UInt computeFullDiscreteSize() const;

    private:
        const UInt nDim;
        UIntArray N;
        RealArray h;
    };


}


#endif //STUDIOSLAB_DIMENSIONMETADATA_H
