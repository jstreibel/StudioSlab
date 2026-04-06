#ifndef STUDIOSLAB_LAB_V2_APP_H
#define STUDIOSLAB_LAB_V2_APP_H

#include "StudioSlab.h"

class FStudioSlabV2App final : public Slab::FApplication {
public:
    FStudioSlabV2App(int argc, const char *argv[]);

protected:
    Slab::TPointer<Slab::Graphics::FGraphicBackend> CreatePlatform() override;
    void OnStart() override;
};

#endif // STUDIOSLAB_LAB_V2_APP_H
