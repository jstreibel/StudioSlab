//
// Created by joao on 28/09/23.
//

#ifndef STUDIOSLAB_REALTIMEANIMATION_H
#define STUDIOSLAB_REALTIMEANIMATION_H

#include "Core/Backend/Modules/Module.h"
#include "Graphics/Modules/GraphicsModule.h"

namespace Slab::Graphics {

    class RealTimeAnimationModule : public GraphicsModule {
    public:
        explicit RealTimeAnimationModule();

        void beginRender() override;
    };

} // Core

#endif //STUDIOSLAB_REALTIMEANIMATION_H
