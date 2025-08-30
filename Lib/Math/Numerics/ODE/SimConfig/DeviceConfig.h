#ifndef DEVICE_H
#define DEVICE_H

#include "Utils/DeviceConfig.h"
#include "Utils/TypesGPU.h"
#include "../../../../Core/Controller/InterfaceOwner.h"
#include "../../../../Core/Controller/Parameter/BuiltinParameters.h"


namespace Slab::Math {
    using namespace Slab::Core;

    constexpr static char PDeviceParamLongName[] = "dev";

    class FDeviceConfig final : public FInterfaceOwner {
        auto NotifyInterfaceSetupIsFinished() -> void override;

        // Device dev = Device::CPU;
        TPointer<IntegerParameter> deviceChoice = New<IntegerParameter>(1, FParameterDescription{PDeviceParamLongName, "Device on which to run simulation.\n"
                                                                             "\t0: CPU \n"
                                                                             "\t1: GPU 0 \n"
                                                                             "\t2: GPU 1 "});
        TPointer<IntegerParameter> nThreads = New<IntegerParameter>(1, FParameterDescription{"th,threads",
                                                               "Number of threads, in case of CPU usage."});

    public:
        explicit FDeviceConfig(bool doRegister = true);

        void setupForThread();

        [[nodiscard]] auto get_nThreads() const -> unsigned int { return **nThreads; }

        bool operator==(const int &RHS) const { return this->GetDevice() == RHS; }

        bool operator!=(const int &RHS) const { return this->GetDevice() != RHS; }

        [[nodiscard]] auto GetDevice() const -> Device;


    };

}

#endif // DEVICE_H
