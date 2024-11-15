//
// Created by joao on 28/09/23.
//

#include "RealTimeAnimation.h"
#include "Animator.h"

namespace Slab::Graphics {

    void RealTimeAnimationModule::Update() {
        GraphicsModule::Update();
        Graphics::Animator::Update();
    }

    RealTimeAnimationModule::RealTimeAnimationModule() : GraphicsModule("Realtime Animation", nullptr) {}

} // Core