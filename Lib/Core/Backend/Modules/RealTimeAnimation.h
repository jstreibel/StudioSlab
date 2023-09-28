//
// Created by joao on 28/09/23.
//

#ifndef STUDIOSLAB_REALTIMEANIMATION_H
#define STUDIOSLAB_REALTIMEANIMATION_H

#include "Module.h"

namespace Core {

    class RealTimeAnimationModule : public Module {
    public:
        void beginRender() override;
    };

} // Core

#endif //STUDIOSLAB_REALTIMEANIMATION_H
