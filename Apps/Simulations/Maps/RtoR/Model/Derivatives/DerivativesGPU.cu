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

typedef thrust::tuple<double,
double,
double> Triple;

typedef thrust::tuple<double,
double,
double,
double> Quadruple;


struct Laplacian1D {
    const double invhsqr;
    explicit Laplacian1D(double h) : invhsqr(1./(h*h)) {}

    __host__ __device__
    inline double operator()(const Triple& d) const {
        const double W = d.get<0>();
        const double C = d.get<1>();
        const double E = d.get<2>();

        return invhsqr * ((W + E) - 2.*C);
    }

} __attribute__((packed)) __attribute__((aligned(8)));


Indices next;
Indices prev;

__host__ DeviceVector &d2dx2(const DeviceVector &in_const,
                             DeviceVector &out,
                             const double h, const size_t N)
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



                             struct Laplacian2D {
    int N, M;
    double invhsqr;
    Laplacian2D(int N, int M, double h) : N(N), M(M), invhsqr(1./(h*h)) {}

    __host__ __device__
    inline double operator()(const thrust::tuple<double, double, double, double, double>& d) const {
        // / x00 x01 x02 \    /     x01     \    /   N   \
        // | x10 x11 x12 |    | x10 x11 x12 |    | W C E |
        // \ x20 x21 x22 /    \     x21     /    \   S   /
        const double x01 = d.get<0>();
        const double x10 = d.get<1>();
        const double x11 = d.get<2>();
        const double x12 = d.get<3>();
        const double x21 = d.get<4>();

        const double vx = x10 + x12;
        const double vy = x01 + x21;

        return invhsqr * ((vx + vy) - 4.*x11);
    }
} __attribute__((aligned(16)));

__host__ DeviceVector &d2dx2(const DeviceVector &in,
                             DeviceVector &out,
                             const double h, const size_t N, const size_t M)
                             {
    (void)M;

    // TODO
    // 1. assert N and M are Po2
    // 2. assert N%P = 0 etc

    auto c0 = in.begin() + 1 + N;
    auto startTuple = thrust::make_tuple(c0-N, c0-1, c0, c0+1, c0+N);
    auto start = thrust::make_zip_iterator(startTuple);

    auto cf = in.end() - 1 - N;
    auto endTuple = thrust::make_tuple(cf-N, cf-1, cf, cf+1, cf+N);
    auto end = thrust::make_zip_iterator(endTuple);

    thrust::transform(start, end, out.begin()+1+N, Laplacian2D(N, M, h));

    return out;
                             }