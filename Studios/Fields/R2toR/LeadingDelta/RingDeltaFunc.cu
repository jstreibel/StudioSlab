

#include "RingDeltaFunc.h"
#include "Phys/Function/DiscreteFunction.h"
#include "Allocator.h"


struct IsRingDeltaDomain
{


    __host__ __device__
    bool operator()(const thrust::tuple<int, int>& t)
    {
        return (thrust::get<0>(t) % 2) == 0;
    }
};


struct RingDeltaGPU
{
    typedef Real argument_type;
    typedef Real result_type;

    const double eps, a_eps, eps_1, t;
    const double rMin, step;
    const int N;
    const double *data;

    RingDeltaGPU(double a, double eps, double t, double rMin, double step, double N, double *data)
    : eps(eps)
    , a_eps(a/eps)
    , eps_1(1./eps)
    , t(t)
    , rMin(rMin)
    , step(step)
    , N(N)
    , data(data)
    {           }

    __device__ Real operator()(int idx) {
        double x = rMin + (idx % N) * step;
        double y = rMin + (idx / N) * step;

        double r = sqrt(x*x + y*y);
        double absarg = abs(r-t);

        if(absarg < eps) return a_eps * (1-absarg*eps_1);

        return data[idx];
    }
};


bool R2toR::LeadingDelta::RingDeltaFunc::renderToDiscreteFunction(Base::DiscreteFunction<Real2D, Real> *toFunc) const {

    auto &outputSpace = toFunc->getSpace();
    const auto N = outputSpace.getDim().getN(0);
    const auto h = outputSpace.geth();
    const auto xMin = Numerics::Allocator::getInstance().getNumericParams().getxLeft();

    thrust::counting_iterator<int> sequence_begin(0);
    thrust::counting_iterator<int> sequence_end(N * N);

    DeviceVector &deviceData = outputSpace.getDeviceData();

    thrust::transform(sequence_begin, sequence_end, deviceData.begin(),
                      RingDeltaGPU(a, eps, radius, xMin, h, N, thrust::raw_pointer_cast(deviceData.data())));

    //thrust::tran

    return true;
}