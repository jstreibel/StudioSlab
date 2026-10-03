#ifndef STUDIOSLAB_LAB_V2_APP_H
#define STUDIOSLAB_LAB_V2_APP_H

#include "StudioSlab.h"
#include "Core/Composition/V2/RuntimeContextV2.h"

class FStudioSlabV2App final : public Slab::FApplication {
    Slab::Core::Composition::V2::FRuntimeContextV2_ptr RuntimeContext;

public:
    FStudioSlabV2App(int argc, const char *argv[]);

protected:
    Slab::TPointer<Slab::Graphics::FGraphicBackend> CreatePlatform() override;
    void OnStart() override;
};

#endif // STUDIOSLAB_LAB_V2_APP_H
