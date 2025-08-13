#include "DerivativesGPU.h"

namespace Slab::Math::R2toR {

    typedef DeviceVector::iterator ElementIterator;
    typedef thrust::device_vector<int> Indices;
    typedef Indices::iterator IndexIterator;
    typedef thrust::permutation_iterator<ElementIterator, IndexIterator> ElementPermutationIterator;

    typedef thrust::tuple<DevFloat, DevFloat, DevFloat> Triple;
    typedef thrust::tuple<DevFloat, DevFloat, DevFloat, DevFloat> Quadruple;


    Indices next;
    Indices prev;


    //*******************************************************************************
    struct Laplacian2D {
        int N, M;
        DevFloat invhsqr;

        Laplacian2D(int N, int M, DevFloat h) : N(N), M(M), invhsqr(1. / (h * h)) {}

        __host__ __device__
        inline DevFloat operator()(const thrust::tuple<DevFloat, DevFloat, DevFloat, DevFloat, DevFloat> &d) const {
            /* Kernel
             *  1  / 0  1  0 \
             * --- | 1  4  1 |
             *  h² \ 0  1  0 /
            */

            /* Campo
             * / x00 x01 x02 \    /     x01     \    /   N   \
             * | x10 x11 x12 |    | x10 x11 x12 |    | W C E |
             * \ x20 x21 x22 /    \     x21     /    \   S   /
             */
            const DevFloat x01 = thrust::get<0>(d);
            const DevFloat x10 = thrust::get<1>(d);
            const DevFloat x11 = thrust::get<2>(d);
            const DevFloat x12 = thrust::get<3>(d);
            const DevFloat x21 = thrust::get<4>(d);

            const DevFloat vx = x10 + x12;
            const DevFloat vy = x01 + x21;

            return invhsqr * ((vx + vy) - 4. * x11);
        }
    } __attribute__((aligned(16)));

    __host__ DeviceVector &d2dx2(const DeviceVector &in,
                                 DeviceVector &out,
                                 const DevFloat h, const size_t N, const size_t M) {
        assert(M == N);

        // TODO
        // 1. assert N and M are Po2
        // 2. assert N%P = 0 etc

        auto c0 = in.begin() + 1 + N;
        auto startTuple = thrust::make_tuple(c0 - N, c0 - 1, c0, c0 + 1, c0 + N);
        auto start = thrust::make_zip_iterator(startTuple);

        auto cf = in.end() - 1 - N;
        auto endTuple = thrust::make_tuple(cf - N, cf - 1, cf, cf + 1, cf + N);
        auto end = thrust::make_zip_iterator(endTuple);

        thrust::transform(start, end, out.begin() + 1 + N, Laplacian2D(N, M, h));

        return out;
    }

}