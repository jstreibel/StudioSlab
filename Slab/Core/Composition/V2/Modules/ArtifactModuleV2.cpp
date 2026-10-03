#include "ArtifactModuleV2.h"

#include "Core/Composition/V2/RuntimeContextV2.h"

namespace Slab::Core::Composition::V2 {

FArtifactModuleV2::FArtifactModuleV2(Artifacts::V2::IArtifactStoreV2_ptr artifactStore)
    : ArtifactStore(std::move(artifactStore)) {
    Descriptor.ModuleId = "artifact.hdf5";
    Descriptor.DisplayName = "HDF5 Artifact Store";
}

auto FArtifactModuleV2::GetDescriptor() const -> const FModuleDescriptorV2& {
    return Descriptor;
}

auto FArtifactModuleV2::RegisterServices(FRuntimeContextV2& context) -> void {
    if (ArtifactStore == nullptr)
        return;
    auto& services = context.GetServices();
    if (!services.Has<Artifacts::V2::IArtifactStoreV2>()) {
        services.Register<Artifacts::V2::IArtifactStoreV2>(ArtifactStore);
    }
}

auto MakeArtifactModuleV2(Artifacts::V2::IArtifactStoreV2_ptr artifactStore) -> IModuleV2_ptr {
    if (artifactStore == nullptr)
        return nullptr;
    return New<FArtifactModuleV2>(std::move(artifactStore));
}

} // namespace Slab::Core::Composition::V2
