#include "DerivativesGPU.h"

#include <thrust/iterator/zip_iterator.h>
#include <thrust/iterator/permutation_iterator.h>
#include <thrust/for_each.h>
#include <thrust/tuple.h>
#include <thrust/transform.h>

typedef DeviceVector::iterator ElementIterator;
typedef thrust::device_vector<int> Indices;
typedef Indices::iterator IndexIterator;
typedef thrust::permutation_iterator<ElementIterator, IndexIterator> ElementPermutationIterator;

typedef thrust::tuple<Real,
Real,
Real> Triple;

typedef thrust::tuple<Real,
Real,
Real,
Real> Quadruple;


struct Laplacian1D {
    const Real invhsqr;
    explicit Laplacian1D(Real h) : invhsqr(1./(h*h)) {}

    __host__ __device__
    inline Real operator()(const Triple& d) const {
        const Real W = d.get<0>();
        const Real C = d.get<1>();
        const Real E = d.get<2>();

        return invhsqr * ((W + E) - 2.*C);
    }

} __attribute__((packed)) __attribute__((aligned(8)));


Indices next;
Indices prev;

__host__ DeviceVector &d2dx2(const DeviceVector &in_const,
                             DeviceVector &out,
                             const Real h, const size_t N)
                             {
    // TODO
    // 1. assert N is Po2
    // 2. assert N%P = 0

    #if PERIODIC_BC
    static bool init = false;
    if(!init){
        thrust::host_vector<int> h_prev(N), h_next(N);

        for (auto i = 0; i < N; ++i) {
            h_prev[i] = i - 1;
            h_next[i] = i + 1;
        }
        h_prev[0] = N - 1;
        h_next[N - 1] = 0;

        next = h_next;
        prev = h_prev;

        init=true;
    }

    auto &in = const_cast<DeviceVector &>(in_const);

    ElementPermutationIterator prevBegin(in.begin(), prev.begin());
    ElementPermutationIterator prevEnd(  in.begin(), prev.end());
    ElementPermutationIterator nextBegin(in.begin(), next.begin());
    ElementPermutationIterator nextEnd(  in.begin(), next.end());

    /* Special thanks to
     *      http://www.mariomulansky.de/data/uploads/cuda_thrust.pdf
     *          Mario Mulansky
     *      http://cs.boisestate.edu/~alark/thrust_intro/presentation.html
     *          Danny George
     *          dannygeorge@u.boisestate.edu
     */

    auto start   = thrust::make_zip_iterator(thrust::make_tuple(prevBegin, in.begin() , nextBegin));
    auto end     = thrust::make_zip_iterator(thrust::make_tuple(prevEnd,   in.end(),    nextEnd));
    auto outBegin = out.begin();

    #else
    const auto &in = in_const;
    auto start   = thrust::make_zip_iterator(thrust::make_tuple(in.begin(), in.begin()+1, in.begin()+2));
    auto end     = thrust::make_zip_iterator(thrust::make_tuple(in.end()-2, in.end()-1, in.end()));
    auto outBegin = out.begin()+1;
    #endif

    thrust::transform(start, end, outBegin, Laplacian1D(h));
    return out;
                             }