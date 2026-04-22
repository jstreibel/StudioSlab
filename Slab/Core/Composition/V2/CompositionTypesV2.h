#ifndef STUDIOSLAB_COMPOSITION_TYPES_V2_H
#define STUDIOSLAB_COMPOSITION_TYPES_V2_H

#include "Utils/Arrays.h"
#include "Utils/Pointer.h"
#include "Utils/String.h"

#include <algorithm>

namespace Slab::Core::Composition::V2 {

    struct FBackendSelectionV2 {
        Str CapabilityId;
        Str BackendId;
    };

    struct FModuleDescriptorV2 {
        Str ModuleId;
        Str DisplayName;
    };

    struct FRuntimeProfileV2 {
        Str ProfileId;
        Str DisplayName;
        Str PlatformHostId;
        Vector<FBackendSelectionV2> BackendSelections;
        Vector<Str> ModuleIds;

        [[nodiscard]] auto FindBackendId(const Str &capabilityId) const -> Str {
            const auto it = std::find_if(
                BackendSelections.begin(),
                BackendSelections.end(),
                [&](const auto &selection) {
                    return selection.CapabilityId == capabilityId;
                });
            if (it == BackendSelections.end()) return {};
            return it->BackendId;
        }
    };

    class FRuntimeContextV2;

    class IModuleV2 {
    public:
        virtual ~IModuleV2() = default;

        [[nodiscard]] virtual auto GetDescriptor() const -> const FModuleDescriptorV2 & = 0;
        virtual auto RegisterServices(FRuntimeContextV2 &context) -> void = 0;
    };

    DefinePointers(IModuleV2)

} // namespace Slab::Core::Composition::V2

#endif // STUDIOSLAB_COMPOSITION_TYPES_V2_H
