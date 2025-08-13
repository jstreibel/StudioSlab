#include "DerivativesGPU.h"

#include <thrust/iterator/zip_iterator.h>
#include <thrust/iterator/permutation_iterator.h>
#include <thrust/for_each.h>
#include <thrust/tuple.h>
#include <thrust/transform.h>

namespace Slab::Math::RtoR {

    typedef DeviceVector::iterator ElementIterator;
    typedef thrust::device_vector<int> Indices;
    typedef Indices::iterator IndexIterator;
    typedef thrust::permutation_iterator<ElementIterator, IndexIterator> ElementPermutationIterator;

    typedef thrust::tuple<DevFloat,
            DevFloat,
            DevFloat> Triple;

    typedef thrust::tuple<DevFloat,
            DevFloat,
            DevFloat,
            DevFloat> Quadruple;


    struct Laplacian1D {
        const DevFloat invhsqr;

        explicit Laplacian1D(DevFloat h) : invhsqr(1. / (h * h)) {}

        __host__ __device__
        inline DevFloat operator()(const Triple &d) const {

            const DevFloat W = thrust::get<0>(d);
            const DevFloat C = thrust::get<1>(d);
            const DevFloat E = thrust::get<2>(d);

            return invhsqr * ((W + E) - 2. * C);
        }

    } __attribute__((packed)) __attribute__((aligned(8)));


    __host__ DeviceVector &d2dx2(const DeviceVector &in_const,
                                 DeviceVector &out,
                                 const DevFloat h, const size_t N, bool periodic) {
        // TODO
        // 1. assert N is Po2
        // 2. assert N%P = 0

        if (periodic) {
            static bool init = false;
            static Indices next;
            static Indices prev;
            if (!init) {
                thrust::host_vector<int> h_prev(N), h_next(N);

                for (auto i = 0; i < N; ++i) {
                    h_prev[i] = i - 1;
                    h_next[i] = i + 1;
                }
                h_prev[0] = N - 1;
                h_next[N - 1] = 0;

                next = h_next;
                prev = h_prev;

                init = true;
            }

            auto &in = const_cast<DeviceVector &>(in_const);

            ElementPermutationIterator prevBegin(in.begin(), prev.begin());
            ElementPermutationIterator prevEnd(in.begin(), prev.end());
            ElementPermutationIterator nextBegin(in.begin(), next.begin());
            ElementPermutationIterator nextEnd(in.begin(), next.end());

            /* Special thanks to
             *      http://www.mariomulansky.de/data/uploads/cuda_thrust.pdf
             *          Mario Mulansky
             *      http://cs.boisestate.edu/~alark/thrust_intro/presentation.html
             *          Danny George
             *          dannygeorge@u.boisestate.edu
             */

            auto start = thrust::make_zip_iterator(thrust::make_tuple(prevBegin, in.begin(), nextBegin));
            auto end = thrust::make_zip_iterator(thrust::make_tuple(prevEnd, in.end(), nextEnd));
            auto outBegin = out.begin();

            thrust::transform(start, end, outBegin, Laplacian1D(h));
        } else {
            const auto &in = in_const;
            auto start = thrust::make_zip_iterator(thrust::make_tuple(in.begin(), in.begin() + 1, in.begin() + 2));
            auto end = thrust::make_zip_iterator(thrust::make_tuple(in.end() - 2, in.end() - 1, in.end()));
            auto outBegin = out.begin() + 1;

            thrust::transform(start, end, outBegin, Laplacian1D(h));
        }


        return out;
    }

}