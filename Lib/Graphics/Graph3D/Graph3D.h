//
// Created by joao on 4/10/23.
//

#ifndef STUDIOSLAB_GRAPH3D_H
#define STUDIOSLAB_GRAPH3D_H

#include "Graphics/Window/Window.h"
#include "Actor.h"
#include "Camera.h"

#include <glm/detail/type_mat4x4.hpp>

#include <list>

namespace Graphics {

    class Graph3D : public Window {
        Camera camera;
        float cameraDist = 9.f;
        float cameraAnglePolar = .25*M_PI;
        float cameraAngleAzimuth = .0f;

        std::list<Actor::Ptr> actors;

        void updateCamera();
    public:
        Graph3D();
        void draw() override;
        auto getCamera() const -> const Camera&;
        bool notifyMouseMotion(int x, int y) override;
        bool notifyMouseWheel(double dx, double dy) override;

        bool addActor(const Actor::Ptr& actor);
    };

} // Graphics

#endif //STUDIOSLAB_GRAPH3D_H
