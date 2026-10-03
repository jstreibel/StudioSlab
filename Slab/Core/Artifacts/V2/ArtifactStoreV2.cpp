#include "ArtifactStoreV2.h"

#ifndef STUDIOSLAB_HDF5_SUPPORT
#define STUDIOSLAB_HDF5_SUPPORT 0
#endif

#if STUDIOSLAB_HDF5_SUPPORT
#include "Core/Artifacts/V2/HDF5ArtifactStoreV2.h"
#endif

namespace Slab::Core::Artifacts::V2 {

auto MakeDefaultArtifactStoreV2() -> IArtifactStoreV2_ptr {
#if STUDIOSLAB_HDF5_SUPPORT
    return New<FHDF5ArtifactStoreV2>();
#else
    return nullptr;
#endif
}

} // namespace Slab::Core::Artifacts::V2
