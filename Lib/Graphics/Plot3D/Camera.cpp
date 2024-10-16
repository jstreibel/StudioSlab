//
// Created by joao on 6/10/23.
//

#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>
#include "Camera.h"

namespace Slab::Graphics {
    auto Camera::getProjection() const -> glm::mat4 {
        // float fovY = 2.0f * glm::atan(d / (2.0f * camHeight));
        // float fovX = 2.0f * glm::atan(w / (2.0f * camHeight));
        // float fov = glm::max(fovX, fovY);
        // float aspect = w / d;  // or use the aspect ratio of your viewport

        float nearPlane = 0.1f;
        float farPlane = 100.0f;

        return glm::perspective(yFov, aspect, nearPlane, farPlane);    }

    auto Camera::getViewTransform() const -> glm::mat4 {
        return glm::lookAt(pos, target, up);
    }
} // Graphics