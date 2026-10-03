#ifndef STUDIOSLAB_ARTIFACT_STORE_V2_H
#define STUDIOSLAB_ARTIFACT_STORE_V2_H

#include "Core/Artifacts/V2/ArtifactTypesV2.h"
#include "Utils/Pointer.h"

#include <filesystem>

namespace Slab::Core::Artifacts::V2 {

class IArtifactStoreV2 {
  public:
    virtual ~IArtifactStoreV2() = default;

    [[nodiscard]] virtual auto SaveRun(const std::filesystem::path& path,
                                       const FArtifactRunV2& run) const -> FResult = 0;
    [[nodiscard]] virtual auto LoadRun(const std::filesystem::path& path) const -> TResult<FArtifactRunV2> = 0;
};

DefinePointers(IArtifactStoreV2)

    [[nodiscard]] auto MakeDefaultArtifactStoreV2() -> IArtifactStoreV2_ptr;

} // namespace Slab::Core::Artifacts::V2

#endif // STUDIOSLAB_ARTIFACT_STORE_V2_H
