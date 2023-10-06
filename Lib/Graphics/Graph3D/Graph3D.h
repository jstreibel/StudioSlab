//
// Created by joao on 4/10/23.
//

#ifndef STUDIOSLAB_GRAPH3D_H
#define STUDIOSLAB_GRAPH3D_H

#include "Graphics/Window/Window.h"
#include "Actor.h"

#include <glm/detail/type_mat4x4.hpp>

#include <list>

namespace Graphics {

    class Graph3D : public Window {
        float cameraDist_xy = 9.f;
        float cameraAngle = -.5*M_PI;
        float cameraHeight = 5;
        float fovY = M_PI * 60.0f / 180.0f;

        std::list<Actor::Ptr> actors;

    public:
        Graph3D();
        void draw() override;
        auto getProjection() const -> glm::mat4;
        auto getViewTransform() const -> glm::mat4;
        bool notifyMouseMotion(int x, int y) override;
        bool notifyMouseWheel(double dx, double dy) override;
    };

} // Graphics

#endif //STUDIOSLAB_GRAPH3D_H
