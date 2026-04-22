#ifndef STUDIOSLAB_LEGACY_RUNTIME_BOOTSTRAP_V2_H
#define STUDIOSLAB_LEGACY_RUNTIME_BOOTSTRAP_V2_H

#include "Core/Composition/V2/RuntimeContextV2.h"

namespace Slab::Core::Composition::V2 {

    enum class ELegacyPlatformHostKindV2 {
        Headless,
        GLFW,
        SFML
    };

    [[nodiscard]] auto GetPlatformHostIdForLegacyPlatformHostKindV2(ELegacyPlatformHostKindV2 kind) -> Str;
    [[nodiscard]] auto GetLegacyBackendIdForPlatformHostKindV2(ELegacyPlatformHostKindV2 kind) -> Str;

    [[nodiscard]] auto MakeLegacyPlatformHostV2(ELegacyPlatformHostKindV2 kind)
        -> Slab::Core::Platform::V2::IPlatformHostV2_ptr;

    [[nodiscard]] auto BuildLegacyRuntimeContextV2(FRuntimeProfileV2 profile,
                                                   ELegacyPlatformHostKindV2 platformHostKind)
        -> FRuntimeContextV2;

    auto EnsureLegacyBackendStartedV2(const Str &legacyBackendId) -> void;
    auto StartupLegacyRuntimeV2(FRuntimeContextV2 &context) -> void;

} // namespace Slab::Core::Composition::V2

#endif // STUDIOSLAB_LEGACY_RUNTIME_BOOTSTRAP_V2_H
