#include "LegacyBridgeModuleV2.h"

#include "Core/Composition/V2/RuntimeContextV2.h"
#include "Core/Composition/V2/Services/ReflectionServiceV2.h"
#include "Core/Composition/V2/Services/TaskServiceV2.h"

namespace Slab::Core::Composition::V2 {

    FLegacyBridgeModuleV2::FLegacyBridgeModuleV2() {
        Descriptor.ModuleId = "legacy.bridge";
        Descriptor.DisplayName = "Legacy Bridge";
    }

    auto FLegacyBridgeModuleV2::GetDescriptor() const -> const FModuleDescriptorV2 & {
        return Descriptor;
    }

    auto FLegacyBridgeModuleV2::RegisterServices(FRuntimeContextV2 &context) -> void {
        auto &services = context.GetServices();

        if (!services.Has<IReflectionServiceV2>()) {
            services.Register<IReflectionServiceV2>(New<FLegacyReflectionServiceV2>());
        }

        if (!services.Has<ITaskServiceV2>()) {
            services.Register<ITaskServiceV2>(New<FLegacyTaskServiceV2>());
        }
    }

    auto MakeLegacyBridgeModuleV2() -> IModuleV2_ptr {
        return New<FLegacyBridgeModuleV2>();
    }

} // namespace Slab::Core::Composition::V2
