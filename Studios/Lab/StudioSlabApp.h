//
// Created by joao on 8/14/25.
//

#ifndef STUDIOSLABAPP_H
#define STUDIOSLABAPP_H

#include "StudioSlab.h"

class LittlePlaneDesigner final : public Slab::FApplication {
public:
    LittlePlaneDesigner(int argc, const char* argv[]);

protected:
    Slab::TPointer<Slab::Graphics::GraphicBackend> CreatePlatform() override;
    void OnStart() override;
};



#endif //STUDIOSLABAPP_H
