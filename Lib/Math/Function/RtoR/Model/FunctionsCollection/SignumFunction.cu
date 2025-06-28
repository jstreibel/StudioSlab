//
// Created by joao on 18/09/2019.
//

#include "SignumFunction.h"

#include "Utils/DeviceConfig.h"


namespace Slab::Math::RtoR {

    struct Sign {
        typedef DevFloat argument_type;
        typedef DevFloat result_type;

        // __thrust_exec_check_disable__
        __host__ __device__ DevFloat

        operator()(const DevFloat &x) const {
            return x < 0.0 ? -1.0 : (x > 0.0 ? 1.0 : 0.0);
        }
    }; // end Sign

    void RtoR::SignumFunction::GPUApply(const DeviceVector &in, DeviceVector &out) const {
        thrust::transform(in.begin(), in.end(),
                          out.begin(), Sign());
    }

}