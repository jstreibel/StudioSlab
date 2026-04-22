#ifndef STUDIOSLAB_RUNTIME_CONTEXT_V2_H
#define STUDIOSLAB_RUNTIME_CONTEXT_V2_H

#include "Core/Composition/V2/CompositionTypesV2.h"
#include "Core/Composition/V2/ServiceRegistryV2.h"
#include "Utils/Arrays.h"
#include "Utils/Pointer.h"

namespace Slab::Core::Platform::V2 {
    class IPlatformHostV2;
    DefinePointers(IPlatformHostV2)
}

namespace Slab::Core::Composition::V2 {

    class FRuntimeContextV2 {
    public:
        explicit FRuntimeContextV2(FRuntimeProfileV2 profile = {});

        [[nodiscard]] auto GetProfile() const -> const FRuntimeProfileV2 &;
        [[nodiscard]] auto GetMutableProfile() -> FRuntimeProfileV2 &;

        auto SetPlatformHost(const Slab::Core::Platform::V2::IPlatformHostV2_ptr &platformHost) -> void;
        [[nodiscard]] auto GetPlatformHost() const -> Slab::Core::Platform::V2::IPlatformHostV2_ptr;

        [[nodiscard]] auto GetServices() -> FServiceRegistryV2 &;
        [[nodiscard]] auto GetServices() const -> const FServiceRegistryV2 &;

        auto InstallModule(const IModuleV2_ptr &module) -> void;
        [[nodiscard]] auto GetInstalledModules() const -> const Vector<IModuleV2_ptr> &;

    private:
        FRuntimeProfileV2 Profile;
        Slab::Core::Platform::V2::IPlatformHostV2_ptr PlatformHost;
        FServiceRegistryV2 Services;
        Vector<IModuleV2_ptr> InstalledModules;
    };

    DefinePointers(FRuntimeContextV2)

} // namespace Slab::Core::Composition::V2

#endif // STUDIOSLAB_RUNTIME_CONTEXT_V2_H
