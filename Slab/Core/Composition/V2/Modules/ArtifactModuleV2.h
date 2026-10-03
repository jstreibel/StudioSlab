#ifndef STUDIOSLAB_ARTIFACT_MODULE_V2_H
#define STUDIOSLAB_ARTIFACT_MODULE_V2_H

#include "Core/Artifacts/V2/ArtifactStoreV2.h"
#include "Core/Composition/V2/CompositionTypesV2.h"

namespace Slab::Core::Composition::V2 {

class FArtifactModuleV2 final : public IModuleV2 {
  public:
    explicit FArtifactModuleV2(Artifacts::V2::IArtifactStoreV2_ptr artifactStore);

    [[nodiscard]] auto GetDescriptor() const -> const FModuleDescriptorV2& override;
    auto RegisterServices(FRuntimeContextV2& context) -> void override;

  private:
    FModuleDescriptorV2 Descriptor;
    Artifacts::V2::IArtifactStoreV2_ptr ArtifactStore;
};

DefinePointers(FArtifactModuleV2)

    [[nodiscard]] auto MakeArtifactModuleV2(Artifacts::V2::IArtifactStoreV2_ptr artifactStore =
                                                Artifacts::V2::MakeDefaultArtifactStoreV2()) -> IModuleV2_ptr;

} // namespace Slab::Core::Composition::V2

#endif // STUDIOSLAB_ARTIFACT_MODULE_V2_H
