//
// Created by joao on 28/09/23.
//

#include "RealTimeAnimation.h"
#include "Animator.h"

namespace Slab::Graphics {

    void FRealTimeAnimationModule::Update() {
        FGraphicsModule::Update();
        Graphics::FAnimator::Update();
    }

    FRealTimeAnimationModule::FRealTimeAnimationModule() : FGraphicsModule("Realtime Animation") {}

} // Core
