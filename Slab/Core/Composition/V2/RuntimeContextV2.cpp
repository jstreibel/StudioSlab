#include "RuntimeContextV2.h"

#include "Core/Platform/V2/PlatformHostV2.h"

#include <algorithm>

namespace Slab::Core::Composition::V2 {

    auto FServiceRegistryV2::Clear() -> void {
        Services.clear();
    }

    auto FServiceRegistryV2::Count() const -> std::size_t {
        return Services.size();
    }

    FRuntimeContextV2::FRuntimeContextV2(FRuntimeProfileV2 profile)
    : Profile(std::move(profile)) {
    }

    auto FRuntimeContextV2::GetProfile() const -> const FRuntimeProfileV2 & {
        return Profile;
    }

    auto FRuntimeContextV2::GetMutableProfile() -> FRuntimeProfileV2 & {
        return Profile;
    }

    auto FRuntimeContextV2::SetPlatformHost(const Slab::Core::Platform::V2::IPlatformHostV2_ptr &platformHost) -> void {
        PlatformHost = platformHost;
        if (PlatformHost != nullptr) {
            Profile.PlatformHostId = PlatformHost->GetPlatformHostId();
        } else {
            Profile.PlatformHostId.clear();
        }
    }

    auto FRuntimeContextV2::GetPlatformHost() const -> Slab::Core::Platform::V2::IPlatformHostV2_ptr {
        return PlatformHost;
    }

    auto FRuntimeContextV2::GetServices() -> FServiceRegistryV2 & {
        return Services;
    }

    auto FRuntimeContextV2::GetServices() const -> const FServiceRegistryV2 & {
        return Services;
    }

    auto FRuntimeContextV2::InstallModule(const IModuleV2_ptr &module) -> void {
        if (module == nullptr) return;

        const auto &descriptor = module->GetDescriptor();
        if (!descriptor.ModuleId.empty()) {
            const auto existing = std::find_if(
                InstalledModules.begin(),
                InstalledModules.end(),
                [&](const auto &installedModule) {
                    return installedModule != nullptr &&
                        installedModule->GetDescriptor().ModuleId == descriptor.ModuleId;
                });
            if (existing != InstalledModules.end()) return;
        }

        InstalledModules.push_back(module);

        if (!descriptor.ModuleId.empty() &&
            !std::ranges::contains(Profile.ModuleIds, descriptor.ModuleId)) {
            Profile.ModuleIds.push_back(descriptor.ModuleId);
        }

        module->RegisterServices(*this);
    }

    auto FRuntimeContextV2::GetInstalledModules() const -> const Vector<IModuleV2_ptr> & {
        return InstalledModules;
    }

} // namespace Slab::Core::Composition::V2
