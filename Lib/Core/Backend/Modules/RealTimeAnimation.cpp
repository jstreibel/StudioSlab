//
// Created by joao on 28/09/23.
//

#include "RealTimeAnimation.h"
#include "Graphics/Animator.h"

namespace Slab::Core {

    void RealTimeAnimationModule::beginRender() {
        Module::beginRender();

        Core::Animator::Update();
    }

} // Core