#include "LegacyPlatformHostV2.h"

#include "Core/Backend/BackendManager.h"
#include "Utils/Exception.h"

#include <utility>

namespace Slab::Core::Platform::V2 {

    namespace {

        [[nodiscard]] auto MakeDescriptor(Str platformHostId,
                                          Str displayName,
                                          const bool bHeadless,
                                          const bool bProvidesGraphics,
                                          const bool bProvidesSystemWindows) -> FPlatformHostDescriptorV2 {
            return FPlatformHostDescriptorV2{
                std::move(platformHostId),
                std::move(displayName),
                bHeadless,
                bProvidesGraphics,
                bProvidesSystemWindows
            };
        }

    } // namespace

    FLegacyBackendPlatformHostV2::FLegacyBackendPlatformHostV2(FPlatformHostDescriptorV2 descriptor, Str legacyBackendId)
    : Descriptor(std::move(descriptor))
    , LegacyBackendId(std::move(legacyBackendId)) {
    }

    auto FLegacyBackendPlatformHostV2::GetDescriptor() const -> const FPlatformHostDescriptorV2 & {
        return Descriptor;
    }

    auto FLegacyBackendPlatformHostV2::Startup() -> void {
        EnsureCompatibleLegacyBackend();
        if (Core::FBackendManager::GetBackendName() == "Uninitialized") {
            Core::FBackendManager::Startup(LegacyBackendId);
        }
    }

    auto FLegacyBackendPlatformHostV2::Run() -> void {
        Startup();
        if (const auto backend = Core::FBackendManager::GetBackend(); backend != nullptr) {
            backend->Run();
        }
    }

    auto FLegacyBackendPlatformHostV2::RequestStop() -> void {
        if (Core::FBackendManager::GetBackendName() == "Uninitialized") return;

        EnsureCompatibleLegacyBackend();
        if (const auto backend = Core::FBackendManager::GetBackend(); backend != nullptr) {
            backend->Terminate();
        }
    }

    auto FLegacyBackendPlatformHostV2::GetLegacyBackendId() const -> const Str & {
        return LegacyBackendId;
    }

    auto FLegacyBackendPlatformHostV2::GetLegacyBackend() const -> Slab::TPointer<Slab::Core::FBackend> {
        if (Core::FBackendManager::GetBackendName() == "Uninitialized") return nullptr;

        EnsureCompatibleLegacyBackend();
        return Core::FBackendManager::GetBackend();
    }

    auto FLegacyBackendPlatformHostV2::EnsureCompatibleLegacyBackend() const -> void {
        const auto currentBackend = Core::FBackendManager::GetBackendName();
        if (currentBackend == "Uninitialized" || currentBackend == LegacyBackendId) return;

        throw Exception(
            "Platform host '" + Descriptor.PlatformHostId +
            "' cannot attach to already initialized legacy backend '" + currentBackend + "'.");
    }

    auto MakeLegacyHeadlessPlatformHostV2() -> FLegacyBackendPlatformHostV2_ptr {
        return New<FLegacyBackendPlatformHostV2>(
            MakeDescriptor("headless", "Legacy Headless", true, false, false),
            "Headless");
    }

    auto MakeLegacyGLFWPlatformHostV2() -> FLegacyBackendPlatformHostV2_ptr {
        return New<FLegacyBackendPlatformHostV2>(
            MakeDescriptor("glfw", "Legacy GLFW", false, true, true),
            "GLFW");
    }

    auto MakeLegacySFMLPlatformHostV2() -> FLegacyBackendPlatformHostV2_ptr {
        return New<FLegacyBackendPlatformHostV2>(
            MakeDescriptor("sfml", "Legacy SFML", false, true, true),
            "SFML");
    }

} // namespace Slab::Core::Platform::V2
