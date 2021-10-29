//
// Created by joao on 18/09/2019.
//

#include "SignumFunction.h"

#include <Common/Workaround/ABIDef.h>
#include <Common/DeviceConfig.h>


struct Sign
{
    typedef Real argument_type;
    typedef Real result_type;

    __thrust_exec_check_disable__
    __host__ __device__ Real operator()(const Real &x) const {
        return x < 0.0 ? -1.0 : (x > 0.0 ? 1.0 : 0.0);
    }
}; // end Sign

void RtoR::SignumFunction::ApplyTo(const DeviceVector &in, DeviceVector &out) const {
    thrust::transform(in.begin(), in.end(),
                      out.begin(), Sign());
}