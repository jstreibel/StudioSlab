#ifndef STUDIOSLAB_LEGACY_PLATFORM_HOST_V2_H
#define STUDIOSLAB_LEGACY_PLATFORM_HOST_V2_H

#include "Core/Backend/Backend.h"
#include "Core/Platform/V2/PlatformHostV2.h"

namespace Slab::Core::Platform::V2 {

    class FLegacyBackendPlatformHostV2 final : public IPlatformHostV2 {
    public:
        FLegacyBackendPlatformHostV2(FPlatformHostDescriptorV2 descriptor, Str legacyBackendId);

        [[nodiscard]] auto GetDescriptor() const -> const FPlatformHostDescriptorV2 & override;

        auto Startup() -> void override;
        auto Run() -> void override;
        auto RequestStop() -> void override;

        [[nodiscard]] auto GetLegacyBackendId() const -> const Str &;
        [[nodiscard]] auto GetLegacyBackend() const -> Slab::TPointer<Slab::Core::FBackend>;

    private:
        auto EnsureCompatibleLegacyBackend() const -> void;

        FPlatformHostDescriptorV2 Descriptor;
        Str LegacyBackendId;
    };

    DefinePointers(FLegacyBackendPlatformHostV2)

    [[nodiscard]] auto MakeLegacyHeadlessPlatformHostV2() -> FLegacyBackendPlatformHostV2_ptr;
    [[nodiscard]] auto MakeLegacyGLFWPlatformHostV2() -> FLegacyBackendPlatformHostV2_ptr;
    [[nodiscard]] auto MakeLegacySFMLPlatformHostV2() -> FLegacyBackendPlatformHostV2_ptr;

} // namespace Slab::Core::Platform::V2

#endif // STUDIOSLAB_LEGACY_PLATFORM_HOST_V2_H
