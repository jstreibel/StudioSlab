//
// Created by joao on 28/09/23.
//

#ifndef STUDIOSLAB_REALTIMEANIMATION_H
#define STUDIOSLAB_REALTIMEANIMATION_H

#include "Core/Backend/Modules/Module.h"
#include "Graphics/Modules/GraphicsModule.h"

namespace Slab::Graphics {

    class FRealTimeAnimationModule : public FGraphicsModule {
    public:
        explicit FRealTimeAnimationModule();

        void Update() override;
    };

    using RealTimeAnimationModule [[deprecated("Use FRealTimeAnimationModule")]] = FRealTimeAnimationModule;

} // Core

#endif //STUDIOSLAB_REALTIMEANIMATION_H
