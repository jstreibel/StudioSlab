

#include "RingDeltaFunc.h"
#include "Math/Function/NumericFunction.h"
#include "Math/Function/R2toR/Model/R2toRNumericFunction.h"

struct IsRingDeltaDomain
{


    __host__ __device__
    bool operator()(const thrust::tuple<int, int>& t)
    {
        return (thrust::get<0>(t) % 2) == 0;
    }
};

struct RingThetaGPU
{
    typedef Slab::Real argument_type;
    typedef Slab::Real result_type;

    const double a, t;
    const double rMin, dx;
    const int N;
    const double *data;

    RingThetaGPU(double a, double t, double rMin, double dx, double N, double *data)
            : a(a)
            , t(t)
            , rMin(rMin)
            , dx(dx)
            , N(N)
            , data(data)
    {           }

    __device__ Slab::Real operator()(int idx) {
        double x = rMin + (idx % N) * dx;
        double y = rMin + (idx / N) * dx;

        double r = sqrt(x*x + y*y);

        if(r-t > -dx) return a;

        return data[idx];
    }
};

struct RingDeltaGPU
{
    typedef Slab::Real argument_type;
    typedef Slab::Real result_type;

    const double eps, a, a_eps, eps_1, t;
    const double rMin, step;
    const int N;
    const double *data;

    RingDeltaGPU(double a, double eps, double t, double rMin, double step, double N, double *data)
    : eps(eps)
    , a(a)
    , a_eps(a/eps)
    , eps_1(1./eps)
    , t(t)
    , rMin(rMin)
    , step(step)
    , N(N)
    , data(data)
    {           }

    __device__ Slab::Real operator()(int idx) {
        double x = rMin + (idx % N) * step;
        double y = rMin + (idx / N) * step;

        double r = sqrt(x*x + y*y);
        double absarg = abs(r-t);

        // if(absarg < eps) return a_eps * (1-absarg*eps_1);
        if(absarg < eps) return a * delta(absarg, eps);

        return data[idx];
    }
};


bool Studios::Fields::R2toRLeadingDelta::RingDeltaFunc::renderToNumericFunction(
        Slab::Math::Base::NumericFunction<Real2D, Real> *toFunc) const {
    auto &func = *dynamic_cast<Slab::Math::R2toR::NumericFunction*>(toFunc);

    auto &outputSpace = toFunc->getSpace();
    fix &meta_data = outputSpace.getMetaData();

    const auto N = meta_data.getN(0);
    const auto h = outputSpace.getMetaData().geth(0);
    const auto xMin = func.getDomain().xMin;

    thrust::counting_iterator<int> sequence_begin(0);
    thrust::counting_iterator<int> sequence_end((int)(N * N));

    DeviceVector &deviceData = outputSpace.getDeviceData();

    auto data = thrust::raw_pointer_cast(deviceData.data());
    if(asTheta)
        thrust::transform(sequence_begin,
                          sequence_end,
                          deviceData.begin(),
                          RingThetaGPU(a, radius, xMin, h, N, data));
    else
        thrust::transform(sequence_begin,
                          sequence_end,
                          deviceData.begin(),
                          RingDeltaGPU(a, eps, radius, xMin, h, N, data));

    return true;
}