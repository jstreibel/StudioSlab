//
// Created by joao on 28/09/23.
//

#include "RealTimeAnimation.h"
#include "Core/Tools/Animator.h"

namespace Core {

    void RealTimeAnimationModule::beginRender() {
        Module::beginRender();

        Core::Animator::Update();
    }

} // Core