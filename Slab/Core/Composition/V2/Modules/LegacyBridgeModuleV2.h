#ifndef STUDIOSLAB_LEGACY_BRIDGE_MODULE_V2_H
#define STUDIOSLAB_LEGACY_BRIDGE_MODULE_V2_H

#include "Core/Composition/V2/CompositionTypesV2.h"

namespace Slab::Core::Composition::V2 {

    class FLegacyBridgeModuleV2 final : public IModuleV2 {
    public:
        FLegacyBridgeModuleV2();

        [[nodiscard]] auto GetDescriptor() const -> const FModuleDescriptorV2 & override;
        auto RegisterServices(FRuntimeContextV2 &context) -> void override;

    private:
        FModuleDescriptorV2 Descriptor;
    };

    DefinePointers(FLegacyBridgeModuleV2)

    [[nodiscard]] auto MakeLegacyBridgeModuleV2() -> IModuleV2_ptr;

} // namespace Slab::Core::Composition::V2

#endif // STUDIOSLAB_LEGACY_BRIDGE_MODULE_V2_H
