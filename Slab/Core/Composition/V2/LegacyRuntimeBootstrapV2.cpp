#include "LegacyRuntimeBootstrapV2.h"

#include "Core/Backend/BackendManager.h"
#include "Core/Platform/V2/LegacyPlatformHostV2.h"
#include "StudioSlab.h"
#include "Utils/Exception.h"

namespace Slab::Core::Composition::V2 {

    auto GetPlatformHostIdForLegacyPlatformHostKindV2(const ELegacyPlatformHostKindV2 kind) -> Str {
        switch (kind) {
        case ELegacyPlatformHostKindV2::Headless:
            return "headless";
        case ELegacyPlatformHostKindV2::GLFW:
            return "glfw";
        case ELegacyPlatformHostKindV2::SFML:
            return "sfml";
        }

        throw Exception("Unknown legacy platform host kind.");
    }

    auto GetLegacyBackendIdForPlatformHostKindV2(const ELegacyPlatformHostKindV2 kind) -> Str {
        switch (kind) {
        case ELegacyPlatformHostKindV2::Headless:
            return "Headless";
        case ELegacyPlatformHostKindV2::GLFW:
            return "GLFW";
        case ELegacyPlatformHostKindV2::SFML:
            return "SFML";
        }

        throw Exception("Unknown legacy platform host kind.");
    }

    auto MakeLegacyPlatformHostV2(const ELegacyPlatformHostKindV2 kind)
        -> Slab::Core::Platform::V2::IPlatformHostV2_ptr {
        using namespace Slab::Core::Platform::V2;

        switch (kind) {
        case ELegacyPlatformHostKindV2::Headless:
            return MakeLegacyHeadlessPlatformHostV2();
        case ELegacyPlatformHostKindV2::GLFW:
            return MakeLegacyGLFWPlatformHostV2();
        case ELegacyPlatformHostKindV2::SFML:
            return MakeLegacySFMLPlatformHostV2();
        }

        throw Exception("Unknown legacy platform host kind.");
    }

    auto BuildLegacyRuntimeContextV2(FRuntimeProfileV2 profile,
                                     const ELegacyPlatformHostKindV2 platformHostKind)
        -> FRuntimeContextV2 {
        FRuntimeContextV2 context(std::move(profile));
        context.SetPlatformHost(MakeLegacyPlatformHostV2(platformHostKind));
        return context;
    }

    auto EnsureLegacyBackendStartedV2(const Str &legacyBackendId) -> void {
        Slab::Startup();

        const auto activeBackendId = Core::FBackendManager::GetBackendName();
        if (activeBackendId == "Uninitialized") {
            Core::FBackendManager::Startup(legacyBackendId);
            return;
        }

        if (activeBackendId != legacyBackendId) {
            throw Exception(
                "Legacy runtime already initialized with backend '" + activeBackendId +
                "', cannot switch to '" + legacyBackendId + "'.");
        }
    }

    auto StartupLegacyRuntimeV2(FRuntimeContextV2 &context) -> void {
        Slab::Startup();

        const auto platformHost = context.GetPlatformHost();
        if (platformHost == nullptr) return;

        platformHost->Startup();
    }

} // namespace Slab::Core::Composition::V2
