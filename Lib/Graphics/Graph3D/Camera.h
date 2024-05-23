//
// Created by joao on 6/10/23.
//

#ifndef STUDIOSLAB_CAMERA_H
#define STUDIOSLAB_CAMERA_H

#include <glm/ext/matrix_float4x4.hpp>
#include "Graphics/Types.h"

namespace Slab::Graphics {

    struct Camera {
    public:
        glm::vec3 pos{0, 1, 0};
        glm::vec3 target{0,0,0};
        glm::vec3 up{0,0,1};

        float aspect=1;
        float yFov=M_PI/3.f;

        auto getProjection() const -> glm::mat4;
        auto getViewTransform() const -> glm::mat4;
    };

} // Graphics

#endif //STUDIOSLAB_CAMERA_H
