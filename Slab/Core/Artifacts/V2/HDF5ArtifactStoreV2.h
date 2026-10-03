#ifndef STUDIOSLAB_HDF5_ARTIFACT_STORE_V2_H
#define STUDIOSLAB_HDF5_ARTIFACT_STORE_V2_H

#include "Core/Artifacts/V2/ArtifactStoreV2.h"

namespace Slab::Core::Artifacts::V2 {

class FHDF5ArtifactStoreV2 final : public IArtifactStoreV2 {
  public:
    [[nodiscard]] auto SaveRun(const std::filesystem::path& path, const FArtifactRunV2& run) const -> FResult override;
    [[nodiscard]] auto LoadRun(const std::filesystem::path& path) const -> TResult<FArtifactRunV2> override;
};

DefinePointers(FHDF5ArtifactStoreV2)

} // namespace Slab::Core::Artifacts::V2

#endif // STUDIOSLAB_HDF5_ARTIFACT_STORE_V2_H
