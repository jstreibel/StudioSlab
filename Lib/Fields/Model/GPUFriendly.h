//
// Created by joao on 18/09/2019.
//

#ifndef V_SHAPE_GPUFRIENDLY_H
#define V_SHAPE_GPUFRIENDLY_H

#include "Lib/Util/device-config.h"

namespace Base {
#if USE_CUDA == true
#define MAKE_GPU_FRIENDLY(CLASS_NAME)                                    \
    public:                                                              \
        CLASS_NAME() : Function(this) {}                                 \
        void ApplyTo(const DeviceVector &in, DeviceVector &out) const override;
#else
#define MAKE_GPU_FRIENDLY(CLASS_NAME)                                    \
    public:                                                              \
        CLASS_NAME() : Function(nullptr) {}
#endif

    template<class PosSpaceType, class TargetSpaceType>
    class Function;

    class GPUFriendly {
    public:
#if USE_CUDA == true
        virtual void ApplyTo(const DeviceVector &in, DeviceVector &out) const = 0;
#else
#endif
    };
}


#endif //V_SHAPE_GPUFRIENDLY_H
