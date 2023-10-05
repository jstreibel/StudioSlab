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
        glm::mat4 projection{1.f};

        float cameraDist_xy = 1.f;
        float cameraAngle = .0f;
        float cameraHeight = 1.0f;

        std::list<Actor::Ptr> actors;

        void updateProjectionMatrix();
    public:
        Graph3D();
        void draw() override;

        auto getProjection() const -> glm::mat4;

        auto getViewTransform() const -> glm::mat4;

        bool notifyMouseMotion(int x, int y) override;

        void notifyReshape(int w, int h) override;
    };

} // Graphics

#endif //STUDIOSLAB_GRAPH3D_H
