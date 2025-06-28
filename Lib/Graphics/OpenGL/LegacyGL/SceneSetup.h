//
// Created by joao on 28/05/24.
//

#ifndef STUDIOSLAB_SCENESETUP_H
#define STUDIOSLAB_SCENESETUP_H

#include "Graphics/Types2D.h"

namespace Slab::Graphics::OpenGL::Legacy {

    void PushScene();

    void ResetModelview();
    void SetupOrtho(const RectR &region, DevFloat zNear=-1.0, DevFloat zFar=1.0);

    void PopScene();
}

#endif //STUDIOSLAB_SCENESETUP_H
